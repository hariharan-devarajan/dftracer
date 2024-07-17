
import dask.dataframe as dd
import re
import os
import glob
import pandas as pd


class DFGrepInterference:
    """
    This class provides methods to manage the graph based representation of IO traces for interference computation.
    init parameters:
    ddf (dask dataframe): dask dataframe (analyzer.events) for computing the metrices.
    app_name (str): Application identifier. Used for reading/writing the checkpoint files.
    cp_dir (str): Checkpoint directory path
    existing (bool): If true, computation can be avoided and the instance can be loaded from checkpoint files for downstream analysis
    """

    def __init__(self, ddf=None, app_name="", cp_dir="", existing=False):
        self.ddf = ddf
        self.ddf_deg = None
        self.inter = None
        self.correlation = None
        self.app_name = app_name
        self.cp_dir = cp_dir
        self.meta = {
            'id': str,
            'name': str,
            'pid': int,
            'size': int,
            'ts': int,
            'te': int,
            'mount_point': str,
            'dur': int,
            'trange': int,
            'deg': int
        }
        if existing:
            self.read_checkpoint(id="inter", cp_dir=cp_dir)
            self.read_checkpoint(id='deg_ddf', cp_dir=cp_dir)
        else:
            self.ddf = self.select_data_cols(
                cols=['id', 'name', 'pid', 'size', 'ts', 'te', 'mount_point', 'dur', 'trange'])

    def select_data_cols(self, cols=[]):
        '''
        returns the data with size > 0 and with selected columns 
        '''
        return self.ddf.query('size > 0')[cols]

    def get_degree(self):
        """
        Calculates the degree for each event. The degree calculation is done on group of events with same mount point and within same timerange.

        get_deg calculates the degree for each group. It first sort the events according to start time, and for each events, look forward to determine if any events have overlapping time. If so, increase the degree by 1 for both events.
        """

        def get_deg(df_group):
            df_group = df_group.sort_values(by='ts').reset_index(
                drop=True)  # check drop true
            group_length = len(df_group)
            degrees = [1]*group_length
            # update degrees by looping throught the group
            for row in range(group_length):
                current_stop_time = df_group.at[row, 'te']
                # look for neighbors
                for neigh in range(row+1, group_length):
                    neigh_start_time = df_group.at[neigh, 'ts']
                    if (neigh_start_time < current_stop_time):
                        degrees[row] += 1
                        degrees[neigh] += 1
                    else:
                        break
            # Add a new column 'deg' representing the count of overlaps
            df_group['deg'] = degrees
            return df_group

        self.ddf_deg = self.ddf.groupby(['mount_point', 'trange']).apply(get_deg, meta=self.meta).reset_index(
            drop=True)  # multiple trange column error while writing so had to drop
        # self.ddf_deg.set_index(['id'])

    def get_interference(self):
        '''
        calculate the interference factor for each events.
        step1: calculate the duration of minimum degree for all size and mount point combination.
        step2: calculate the Interference factor based on duration of the event and the duration of min degree event. 
        '''
        def dur_of_min_deg(ddf):
            '''
            calculate the duration of minimum degree for all size and mount point combination.
            '''
            ddf1 = ddf.copy()
            list_deg = ddf1.groupby(["mount_point", "size"])[
                "deg"].min().compute()
            agg_dict = {}
            for deg in list_deg:
                agg_dict[str(deg)] = min
                ddf1[str(deg)] = 9223372036854775807
                ddf1[str(deg)] = ddf1[str(deg)].mask(
                    ddf1['deg'] == deg, ddf1['dur'])
            return ddf1, agg_dict, list_deg

        def calculate_interference(ddf, agg_dict, list_deg):
            '''
            calculate the Interference factor based on duration of the event and the duration of min degree event. 
            '''
            agg_dict["deg"] = min
            val = ddf.groupby(['size', 'mount_point']).agg(agg_dict)
            val['min_dur'] = 0
            for deg in list_deg:
                val['min_dur'] = val['min_dur'].mask(
                    val['deg'].eq(deg), val[str(deg)])
            ddf2 = val.reset_index()
            merge = ddf.merge(ddf2, on=['size', 'mount_point'], how='left', suffixes=('_caller', '_other'))[
                ['name', 'pid', 'size', 'ts', 'te', 'mount_point', 'dur', 'trange', 'deg_caller', 'deg_other', 'min_dur']]
            merge['interference'] = merge['min_dur']/merge['dur']
            return merge

        dft1, agg_dict, list_deg = dur_of_min_deg(self.ddf_deg)
        self.inter = calculate_interference(
            dft1, agg_dict=agg_dict, list_deg=list_deg)

    def write_checkpoint(self, id, cp_dir):
        '''
        write the datafame as parquet files
        '''
        write_df = getattr(self, id)
        # print(write_df.compute())
        # schema = {'id': int, 'name': str, 'pid': int, 'size': int, 'ts': int, 'te': int, 'mount_point': str, 'dur': int, 'trange': int, 'deg':int}
        write_df.to_parquet(f"{cp_dir}/{self.app_name}",
                            name_function=lambda i: f'{id}-{i}.parquet')

    def read_checkpoint(self, id, cp_dir):
        '''
        read the dataframe from checkpoint files
        '''
        read_df = dd.read_parquet(f"{cp_dir}/{self.app_name}/{id}*.parquet")
        setattr(self, id, read_df)


class DFGrepWorkflow:
    """
    This class provides methods to represent the IO traces as workflow graphs.
    init parameters: 
    """

    def __init__(self, ddf=None, app_name="", trace_path=""):
        self.ddf = ddf
        self.app_name = app_name
        self.trace_path = trace_path

    def select_cols(self, cols=[]):
        return self.ddf[cols]

    def get_pid_map(self):
        '''
        This function is designed for mummi traces to map pid with the application based on the filename
        '''
        all_files = glob.glob(self.trace_path)
        pid_map = {}
        for file in all_files:
            slices = os.path.basename(file).split('.')
            if (len(slices) > 4):
                pid_map[slices[3]] = slices[1]
        return pid_map

    def create_workflow(self):
        '''
        1. Find number of times each file is prod/cons. And select the files that are both prod & cons atleast once
        2. Get the list of the files which are both produced and consumed and select only the events with these files
        '''
        prod_cons = self.ddf.groupby('filename')['prod', 'cons'].sum()
        prod_cons = prod_cons.query('prod > 0 and cons > 0').reset_index()
        filelist = prod_cons.filename.unique().compute()
        selected_events = self.ddf[self.ddf.filename.isin(filelist)]
        selected_events_sum = selected_events.groupby(['filename', 'pid']).agg(
            {'prod': 'sum', 'cons': 'sum', 'ts': 'min'}).reset_index()
        merged = selected_events_sum.merge(
            prod_cons, on=["filename"], how='left', suffixes=['_pid', '_fid'])
        final = merged.query(
            'not (prod_pid == prod_fid and cons_pid == cons_fid)')
        return final

    def create_graph_df(self, df, pid_map):
        '''
        Function creates soruce and destination data for plotting the graph. This version is currently designed for mummi workflow
        '''
        def get_base_filename(path):
            return os.path.basename(path)

        def process_row(row):
            filename = re.sub("\d+", "x", row['filename'])
            filename = "f_"+get_base_filename(filename)
            # filename = row['filename']
            pid = pid_map[str(row['pid'])] if str(
                row['pid']) in pid_map else str(row['pid'])
            pid = "p_"+pid
            prod = row['prod_pid']
            cons = row['cons_pid']

            if prod == 0:
                return [{'src': filename, 'dest': pid, 'wt': row['ts']}]

            elif cons == 0:
                return [{'src': pid, 'dest': filename, 'wt': row['ts']}]
            elif prod > 0 and cons > 0:
                return [{'src': filename, 'dest': pid, 'wt': row['ts']}, {'src': pid, 'dest': filename, 'wt': row['ts']}]

        graph_df = pd.DataFrame([item for sublist in df.apply(
            process_row, axis=1) for item in sublist])
        return graph_df
