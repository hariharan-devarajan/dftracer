from glob import glob
import pandas as pd
print(f"pd {pd.__version__}")
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
args = parser.parse_args()
filename = args.trace_file

file_pattern = glob(filename)

all_records = []
start = time.time()
for file in file_pattern:
    for record in read_trace(file):
        all_records.append(all_records)

pd.DataFrame.from_dict(all_records, orient='columns')

end = time.time()
print(f"Loading Recorder trace took {end-start} seconds.")