from glob import glob
import pandas as pd
print(f"pd {pd.__version__}")
<<<<<<< HEAD
=======

>>>>>>> d419e7e (Paper/ad ae (#100))
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

import recorder_viz
from recorder_viz import RecorderReader

logging.basicConfig(filename='darshan_main.log', encoding='utf-8', level=logging.DEBUG)


def get_json(func, ts, dur, rank):
    d = {}
    #print(location, start)
    d["name"] = func
    d["cat"] = "Recorder"
    d["ts"] = int(ts)
    d["dur"] = int(dur)
    d["pid"] = rank
    d["tid"] = 0
    #print(d)
    return d


def read_trace(trace_name):
    map_events = {}
    count = 0
    reader = RecorderReader(trace_name)
    func_list = reader.funcs
    for rank, records in enumerate(reader.records):
        lm = reader.LMs[rank]
        for record in records:
            if len(func_list) > record.func_id:
                func_name = func_list[record.func_id]
                if record.func_id > 0 and "MPI" not in func_name:
                    yield get_json(func_name, 0, 10, rank)

parser = argparse.ArgumentParser(
    description="Time functions and print time spent in each function",
    formatter_class=argparse.RawDescriptionHelpFormatter,
)
parser.add_argument("trace_file", help="Trace file to load", type=str)
<<<<<<< HEAD
=======

>>>>>>> d419e7e (Paper/ad ae (#100))
parser.add_argument("--workers", help="Number of workers", type=int, default=1)
args = parser.parse_args()
filename = args.trace_file

cluster = LocalCluster(n_workers=args.workers)  # Launches a scheduler and workers locally
client = Client(cluster)  # Connect to distributed cluster and override default

file_pattern = glob(filename)

all_records = []
start = time.time()

create_bag = dask.bag.from_delayed([dask.delayed(read_trace)(file) 
                                                for file in file_pattern])
columns = {'name':"string", 'cat': "string",
           'pid': "string",'tid': "string",
           'dur': "uint64", 'ts': "uint64"}
events = create_bag.to_dataframe(meta=columns)
#events.head()
n_partition = 1
events = events.repartition(npartitions=n_partition).persist()
progress(events)
_ = wait(events)

end = time.time()
print(f"Loading Recorder trace took {end-start} seconds.")