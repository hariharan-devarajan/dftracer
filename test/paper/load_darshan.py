from glob import glob
import pandas as pd
print(f"pd {pd.__version__}")
import dask
import dask.dataframe as dd
from dask.distributed import Client, LocalCluster, progress, wait
print(f"dask {dask.__version__}")
import pyarrow as pa
print(f"pa {pa.__version__}")
import logging
from glob import glob
import argparse
import time

import darshan
logging.basicConfig(filename='darshan_main.log', encoding='utf-8', level=logging.DEBUG)

def generate_darshan_records(log_file):
    def get_dict(row):
        d = {}
        d["size"] = row["length"]
        d["ts"] = int(row["start_time"] * 10e6)
        d["dur"] = int(row["end_time"] * 10e6) -  d["ts"]
        d["tinterval"] = I.to_string(I.closed(d["ts"] , d["ts"] + d["dur"]))
        d["trange"] = int(((d["ts"] + d["dur"])/2.0) / time_granularity)
        d["phase"] = 2
        d["compute_time"] = I.to_string(I.empty())
        d["io_time"] = d["tinterval"]
        return d
    report = darshan.DarshanReport(log_file, read_all=True)
    if "DXT_POSIX" in report.modules:
        time_granularity=10e3
        for val in report.records['DXT_POSIX'].to_df():
            d = {}
            fileid = val["id"]
            write_df = val["write_segments"]
            read_df = val["read_segments"]
            d["hostname"] = val["hostname"]
            d["pid"] = val["rank"]
            d["tid"] = 0
            d["cat"] = "POSIX"
            d["filename"] = report.data['name_records'][fileid]
            for index, row in write_df.iterrows():
                d["name"] = "write"
                d.update(get_dict(row))
                yield d
            for index, row in read_df.iterrows():
                d["name"] = "read"
                d.update(get_dict(row))
                yield d

parser = argparse.ArgumentParser(
    description="Time functions and print time spent in each function",
    formatter_class=argparse.RawDescriptionHelpFormatter,
)
parser.add_argument("trace_file", help="Trace file to load", type=str)
parser.add_argument("--workers", help="Number of workers", type=int, default=1)
args = parser.parse_args()
filename = args.trace_file

cluster = LocalCluster(n_workers=args.workers)  # Launches a scheduler and workers locally
client = Client(cluster)  # Connect to distributed cluster and override default

args = parser.parse_args()
filename = args.trace_file

cluster = LocalCluster(n_workers=args.workers)  # Launches a scheduler and workers locally
client = Client(cluster)  # Connect to distributed cluster and override default

args = parser.parse_args()
filename = args.trace_file


file_pattern = glob(filename)

all_records = []
start = time.time()

create_bag = dask.bag.from_delayed([dask.delayed(generate_darshan_records)(file) 
                                                for file in file_pattern])
columns = {'name':"string[pyarrow]", 'cat': "string[pyarrow]",
            'pid': "uint64[pyarrow]",'tid': "uint64[pyarrow]",
            'dur': "uint64[pyarrow]", 'tinterval': "string[pyarrow]",
            'trange': "uint64[pyarrow]", 'hostname': "string[pyarrow]",
            'compute_time': "string[pyarrow]", 'io_time': "string[pyarrow]",
            'filename': "string[pyarrow]", 'phase': "uint16[pyarrow]",
            'size': "uint64[pyarrow]"}
events = create_bag.to_dataframe(meta=columns)

n_partition = 1
events = events.repartition(npartitions=n_partition).persist()
progress(events)
_ = wait(events)

end = time.time()
print(f"Loading Darshan trace took {end-start} seconds.")