from glob import glob
import pandas as pd
import argparse
import dask
import dask.dataframe as dd
import pyarrow as pa
import numpy as np
from itertools import chain

from dask.distributed import Client, LocalCluster, progress, wait
from dask.distributed import Future, get_client
from typing import Tuple, Union
import os
import intervals as I
import math
import re
from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.tree import Tree
import subprocess
import json
import logging

import zindex_py as zindex
HOST_PATTERN=r'corona(\d+)'
ZINDEX_BIN="/usr/WS2/iopp/software/zindex/build/Release"
REBUILD_INDEX=False
BATCH=1024**2
DELIMITER=";"
def create_index(filename):
    directory = os.path.dirname(filename)
    index_file = os.path.join(directory, "index", f"{filename}.zindex")
    if not os.path.exists(index_file):
        status = zindex.create_index(filename, index_file=f"file:{index_file}",
                                     regex="id:\b([0-9]+)", numeric=True, unique=True, debug=True)
        logging.debug(f"Creating Index for {filename} returned {status}")
    return filename

def get_linenumber(filename):
    directory = os.path.dirname(filename)
    index_file = os.path.join(directory, "index", f"{filename}.zindex")
    line_number = zindex.get_max_line(filename, index_file=index_file)
    logging.debug(f" The {filename} has {line_number} lines")
    return DELIMITER.join([filename, str(line_number)])

def get_size(filename):
    directory = os.path.dirname(filename)
    if filename.endswith('.pfw'):
        size = os.stat(filename).st_size
    elif filename.endswith('.pfw.gz'):
        index_file = os.path.join(directory, "index", f"{filename}.zindex")
        size = zindex.get_total_size(filename, index_file=index_file)
    logging.debug(f" The {filename} has {size/1024**3} GB size")
    return int(size)


def generate_line_batches(args):
    filename, max_line = args.split(DELIMITER)
    max_line = int(max_line)
    #print(args)
    for start in range(0, max_line, BATCH):
        logging.debug(f"Created a batch for {filename} from [{start}, {start + BATCH}] lines")
        yield DELIMITER.join([filename, str(start)])

def load_indexed_gzip_files(args):
    for arg in args:
        filename, start = arg.split(DELIMITER)
        directory = os.path.dirname(filename)
        index_file = os.path.join(directory, "index", f"{filename}.zindex")
        start = int(start)
        json_lines = zindex.zquery(filename, index_file=index_file,
                              raw=f"select a.line from LineOffsets a where a.line >= {start} AND a.line < {start+BATCH};", debug=True)
        for json_line in json_lines:
            logging.debug(f"Found an {json_line} line for {filename} range [{start}, {start + BATCH}] lines")
            yield json_line

def load_objects(line, fn, time_granularity):
    d = {}
    if line is not None and line !="" and "[" != line[0] and line != "\n" :
        val = {}
        try:
            val = json.loads(line)
            logging.debug(f"Loading dict {val}")
            if "name" in val:
                if "id" in val:
                    d["index"] = val["id"]
                else:
                    d["index"] = 0
                d["name"] = val["name"]
                d["cat"] = val["cat"]
                d["pid"] = val["pid"]
                d["tid"] = val["tid"]
                val["dur"] = int(val["dur"])
                val["ts"] = int(val["ts"])
                d["dur"] = val["dur"]
                d["tinterval"] = I.to_string(I.closed(val["ts"] , val["ts"] + val["dur"]))
                d["trange"] = int(((val["ts"] + val["dur"])/2.0) / time_granularity)
                d.update(io_function(val, d))
                if fn:
                    d.update(fn(val, d))
                logging.debug(f"built an dictionary for line {d}")
        except ValueError as error:
            logging.error(f"Processing {line} failed with {error}")
    return d
def io_function(json_object, current_dict):
    d = {}
    if "compute" in json_object["name"]:
        d["compute_time"] = current_dict["tinterval"]
        d["phase"] = 1
    else:
        d["compute_time"] = I.to_string(I.empty())
    if "POSIX" in json_object["cat"]:
        d["io_time"] = current_dict["tinterval"]
        d["phase"] = 2
    elif "reader" in json_object["cat"]:
        d["io_time"] = current_dict["tinterval"]
        d["phase"] = 3
    else:
        d["io_time"] = I.to_string(I.empty())
    if "args" in json_object:
        if "fname" in json_object["args"]:
            d["fname"] = json_object["args"]["fname"]
        if "hostname" in json_object["args"]:
            d["hostname"] = json_object["args"]["hostname"]

        if "POSIX" == json_object["cat"] and "ret" in json_object["args"]:
            if "write" in json_object["name"]:
                d["size"] = int(json_object["args"]["ret"])
            elif "read" in json_object["name"] and "readdir" not in json_object["name"]:
                d["size"] = int(json_object["args"]["ret"])
        else:
            if "image_size" in json_object["args"]:
                d["size"] = int(json_object["args"]["image_size"])
    return d

def io_columns():
    return {
        'hostname': "string[pyarrow]",
        'compute_time': "string[pyarrow]",
        'io_time': "string[pyarrow]",
        'app_io_time': "string[pyarrow]",
        'filename': "string[pyarrow]",
        'phase': "uint16[pyarrow]",
        'size': "uint64[pyarrow]"
    }

def group_func(df):
    val = I.empty()
    for index, value in df.items():
        if str(value) != 'NA':
            val = val.union(I.from_string(str(value), int))
    logging.debug(f"Grouped Range into {val}")
    return I.to_string(val)

def union_portions():
    return dd.Aggregation(
        'union_portions',
        chunk=lambda s: s.apply(group_func),
        agg=lambda s: s.apply(group_func)
    )
def difference_portion(df, a, b):
    if str(df[a]) != 'NA' and str(df[b]) != 'NA':
        return I.to_string(I.from_string(str(df[a]), int) - I.from_string(str(df[b]), int))
    elif str(df[a]) != 'NA':
        return df[a]
    else:
        return I.to_string(I.empty())
def size_portion(df, col):
    val = 0.0
    if str(df[col]) == 'NA':
        return val
    ia = I.from_string(str(df[col]), int)
    for i in list(ia):
        if i and not i.is_empty():
            val += i.upper - i.lower
    logging.debug(f"Calculating size of Interval {val}")
    return val
def percentile(n):
    return dd.Aggregation(
        name='percentile_{:02.0f}'.format(n*100),
        # this computes the median on each partition
        chunk=lambda s: s.quantile(n),
        # this combines results across partitions; the input should just be a list of length 1
        agg=lambda s0: s0.quantile(n),
    )
median_fun = dd.Aggregation(
    name="median",
    # this computes the median on each partition
    chunk=lambda s: s.median(),
    # this combines results across partitions; the input should just be a list of length 1
    agg=lambda s0: s0.median(),
)
def human_format(num):
    if num:
        num = float('{:.3g}'.format(num))
        magnitude = 0
        while abs(num) >= 1024:
            magnitude += 1
            num /= 1024.0
        return '{}{}'.format('{:.0f}'.format(num).rstrip('.'), ['', 'KB', 'MB', 'GB', 'TB'][magnitude])
    else:
        return "NA"

class DLPAnalyzer:

    def __init__(self, file_pattern, time_granularity=10e3, load_fn=None, load_cols={}):
        file_pattern = glob(file_pattern)
        all_files = []
        pfw_pattern = []
        pfw_gz_pattern = []
        for file in file_pattern:
            if file.endswith('.pfw'):
                pfw_pattern.append(file)
                all_files.append(file)
            elif file.endswith('.pfw.gz'):
                pfw_gz_pattern.append(file)
                all_files.append(file)
            else:
                logging.warn(f"Ignoring unsuported file {file}")
        logging.debug(f"Processing files {all_files}")
        create_bag = dask.bag.from_sequence(file_pattern).map(create_index).compute()
        total_size = dask.bag.from_sequence(all_files).map(get_size).sum().compute()
        file_meta_bag = dask.bag.from_delayed([dask.delayed(get_linenumber)(file)
                                               for file in file_pattern])
        line_batch_bag = dask.bag.from_delayed([dask.delayed(generate_line_batches)(file_meta_task)
                                                for file_meta_task in file_meta_bag.to_delayed()])
        json_line_bag = dask.bag.from_delayed([dask.delayed(load_indexed_gzip_files)(line_batch_task)
                                               for line_batch_task in line_batch_bag.to_delayed()])
        gz_bag = json_line_bag.map(load_objects, fn=load_fn, time_granularity=time_granularity).filter(
            lambda x: "name" in x)
        pfw_bag = dask.bag.read_text(pfw_pattern).map(load_objects, fn=load_fn, time_granularity=time_granularity).filter(lambda x: "name" in x)
        main_bag = dask.bag.concat([pfw_bag, gz_bag])
        columns = {'index': "uint64[pyarrow]",
                   'name': "string[pyarrow]", 'cat': "string[pyarrow]",
                   'pid': "uint64[pyarrow]", 'tid': "uint64[pyarrow]",
                   'dur': "uint64[pyarrow]",
                   'tinterval': "string[pyarrow]", 'trange': "uint64[pyarrow]"}
        columns.update(io_columns())
        columns.update(load_cols)
        events = main_bag.to_dataframe(meta=columns)
        n_partition = math.ceil(total_size / (32 * 1024 ** 3))
        logging.debug(f"Number of partitions used are {n_partition}")
        self.events = events.repartition(npartitions=n_partition).persist()
        _ = wait(self.events)
        logging.info(f"Loaded events")

    def _calculate_time(self):
        agg = {"compute_time": union_portions(),
               "io_time": union_portions(),
               "app_io_time": union_portions(),
               "tinterval": union_portions()}
        grouped_df = self.events.groupby("trange").agg(agg)
        grouped_df["only_io"] = grouped_df[["io_time", "compute_time"]].apply(difference_portion, a="io_time",
                                                                              b="compute_time", axis=1,
                                                                              meta=("string[pyarrow]"))
        grouped_df["only_compute"] = grouped_df[["io_time", "compute_time"]].apply(difference_portion, a="compute_time",
                                                                                   b="io_time", axis=1,
                                                                                   meta=("string[pyarrow]"))
        grouped_df["only_app_io"] = grouped_df[["app_io_time", "compute_time"]].apply(difference_portion, a="app_io_time",
                                                                              b="compute_time", axis=1,
                                                                              meta=("string[pyarrow]"))
        grouped_df["only_app_compute"] = grouped_df[["app_io_time", "compute_time"]].apply(difference_portion, a="compute_time",
                                                                                   b="app_io_time", axis=1,
                                                                                   meta=("string[pyarrow]"))
        total_time, total_io_time, total_compute_time, total_app_io_time,\
        only_io, only_compute, only_app_io, only_app_compute = dask.compute(
            grouped_df[["tinterval"]].apply(size_portion, col="tinterval", axis=1).sum(),
            grouped_df[["io_time"]].apply(size_portion, col="io_time", axis=1).sum(),
            grouped_df[["compute_time"]].apply(size_portion, col="compute_time", axis=1).sum(),
            grouped_df[["app_io_time"]].apply(size_portion, col="app_io_time", axis=1).sum(),
            grouped_df[["only_io"]].apply(size_portion, col="only_io", axis=1).sum(),
            grouped_df[["only_compute"]].apply(size_portion, col="only_compute", axis=1).sum(),
            grouped_df[["only_app_io"]].apply(size_portion, col="only_app_io", axis=1).sum(),
            grouped_df[["only_app_compute"]].apply(size_portion, col="only_app_compute", axis=1).sum(),

        )
        logging.debug(f"{total_time}, {total_io_time}, {total_compute_time}, {total_app_io_time}, \
               {only_io}, {only_compute}, {only_app_io}, {only_app_compute}")
        return total_time, total_io_time, total_compute_time, total_app_io_time, \
               only_io, only_compute, only_app_io, only_app_compute

    def _create_interval(self, list_items):
        logging.debug(f"Creating interval from {list_items}")
        prev = list_items[0]
        val = I.closed(prev, prev)
        for proc in list_items[1:]:
            val = val | I.closed(prev, proc)
            prev = proc
        logging.info(f"Created an interval of {val}")
        return val

    def _create_host_intervals(self, hosts_list):
        logging.debug(f"Creating regex for {hosts_list}")
        is_first = True
        value = I.empty()
        for host in hosts_list:
            val = int(re.findall(HOST_PATTERN, host)[0])
            if is_first:
                prev = val
                is_first = False
                value = I.closed(prev, prev)
            else:
                value = value | I.closed(prev, val)
        val = re.findall(HOST_PATTERN, hosts_list[0])[0]
        regex = hosts_list[0].replace(val, str(value))
        logging.info(f"Created regex value {val}")
        return regex

    def _remove_numbers(self, string_items):
        logging.debug(f"Removing numbers from {string_items}")
        item_sets = set()
        for file in string_items:
            item_sets.add(re.sub(r'\d', 'X', str(file)))
        logging.info(f"List after removing numbers {list(item_sets)}")
        return list(item_sets)

    def summary(self):
        total_time, total_io_time, total_compute_time, total_app_io_time, \
        only_io, only_compute, only_app_io, only_app_compute = self._calculate_time()
        hosts_used, filenames_accessed, num_procs, compute_tid, posix_tid, io_by_operations = dask.compute(
            self.events["hostname"].unique(),
            self.events["filename"].unique(),
            self.events["pid"].unique(),
            self.events.query("phase == 1")["tid"].unique(),
            self.events.query("phase == 2")["tid"].unique(),
            self.events.query("phase == 2").groupby(["name"]).agg(
                {"dur": [sum, "count"], "size": [sum, "mean", median_fun, min, max, percentile(.25), percentile(.75)]})
        )
        num_events = len(self.events)

        hosts_used = hosts_used.to_list()
        hosts_used_regex_str = self._create_host_intervals(hosts_used)

        filenames_accessed = filenames_accessed.to_list()
        filename_basename_regex_str = self._remove_numbers(filenames_accessed)

        num_procs = num_procs.to_list()
        proc_name_regex = self._create_interval(num_procs)

        io_by_ops_dict = io_by_operations.T.to_dict()

        # Create a new Table object from Rich library
        table = Table(box=None, show_header=False)

        # Add columns to the table for the key and value
        table.add_column(style="cyan")
        table.add_column()
        app_tree = Tree("Scheduler Allocation Details")
        app_tree.add(f"Nodes: {str(len(hosts_used))} {hosts_used_regex_str}")
        app_tree.add(f"Processes: {str(len(num_procs))} {str(proc_name_regex)}")
        thread_tree = Tree("Thread allocations across nodes (includes dynamically created threads)")
        thread_tree.add(f"Compute: {str(len(compute_tid))}")
        thread_tree.add(f"I/O: {str(len(posix_tid))}")
        app_tree.add(thread_tree)
        app_tree.add(f"Events Recorded: {str(num_events)}")
        table.add_row("Allocation", app_tree)

        data_tree = Tree("Description of Dataset Used")
        data_tree.add(f"Files: {str(len(filenames_accessed))} {filename_basename_regex_str}")
        table.add_row("Dataset", data_tree)

        io_tree = Tree("Behavior of Application")
        io_time = Tree("Split of Time in application")
        io_time.add(f"Compute: {total_compute_time / 1e6:.3f} sec")
        io_time.add(f"Overall I/O: {total_io_time / 1e6:.3f} sec")
        io_time.add(f"Unoverlapped I/O: {only_io / 1e6:.3f} sec")
        io_time.add(f"Unoverlapped Compute: {only_compute / 1e6:.3f} sec")
        io_tree.add(io_time)
        padding_size = 6
        key_padding_size = 15
        io_ts = Tree("Transfer size distribution by function")
        io_ts.add(
            f"{'Function':<{key_padding_size}}|{'min':<{padding_size}}|{'25':<{padding_size}}|{'mean':<{padding_size}}|{'median':<{padding_size}}|{'75':<{padding_size}}|{'max':<{padding_size}}|")
        for key, value in io_by_ops_dict.items():
            if "close" not in key or "open" not in key:
                io_ts.add(
                    f"{key.split('.')[-1]:<{key_padding_size}}|{human_format(value[('size', 'min')]):<{padding_size}}|{human_format(value[('size', 'percentile_25')]):<{padding_size}}|{human_format(value[('size', 'mean')]):<{padding_size}}|{human_format(value[('size', 'median')]):<{padding_size}}|{human_format(value[('size', 'percentile_75')]):<{padding_size}}|{human_format(value[('size', 'max')]):<{padding_size}}|")
        io_tree.add(io_ts)
        io_ops = Tree("Event count by function")
        for key, value in io_by_ops_dict.items():
            io_ops.add(f"{key.split('.')[-1]} : {value[('dur', 'count')]}")
        io_tree.add(io_ops)
        table.add_row("I/O Behavior", io_tree)
        console = Console()

        # Print the table with Rich formatting
        console.print(Panel(table, title='Summary'))


def parse_args():
    parser = argparse.ArgumentParser(description='DLIO Profiler Analyzer')
    parser.add_argument("trace", type=str,
                        help="Path to trace file from DLIO Profiler. Can contain * for multiple files.")
    parser.add_argument('-d', '--debug', help="Print lots of debugging statements",
                        action="store_const", dest="loglevel", const=logging.DEBUG, default=logging.WARNING)
    parser.add_argument('-v', '--verbose', help="Be verbose", action="store_const", dest="loglevel", const=logging.INFO)
    parser.add_argument("-l","--log-file", default="dlp_analyzer_main.log", type=str, help="Logging log file")
    parser.add_argument("-w","--workers", default=16, type=int, help="Number of dask workers to use")
    args = parser.parse_args()
    return args


def print_versions():
    logging.debug(f"pandas version {pd.__version__}")
    logging.debug(f"dask version {dask.__version__}")
    logging.debug(f"pa version {pa.__version__}")
    logging.debug(f"np version {np.__version__}")


def setup_logging(args):
    logging.basicConfig(level=args.loglevel,
        handlers=[
            logging.FileHandler(args.log_file, mode="a", encoding='utf-8'),
            logging.StreamHandler()
        ],
        format='[%(levelname)s] %(message)s [%(pathname)s:%(lineno)d]'
    )

def setup_dask_cluster(args):
    cluster = LocalCluster(n_workers=args.workers)  # Launches a scheduler and workers locally
    client = Client(cluster)  # Connect to distributed cluster and override default
    logging.info(f"Initialized Client with {args.workers} workers")

def main():
    args = parse_args()
    setup_logging(args)
    logging.debug(args)
    setup_dask_cluster(args)
    analyzer = DLPAnalyzer(args.trace)
    analyzer.summary()

if __name__ == '__main__':
    main()
    exit(0)
