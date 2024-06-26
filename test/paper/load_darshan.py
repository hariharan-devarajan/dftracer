from glob import glob
import pandas as pd
print(f"pd {pd.__version__}")
import logging
import darshan
from glob import glob
import argparse
import time

print(f"darshan {darshan.__version__}")

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
args = parser.parse_args()
filename = args.trace_file

file_pattern = glob(filename)

all_records = []
start = time.time()
for file in file_pattern:
    for record in generate_darshan_records(file):
        all_records.append(all_records)

pd.DataFrame.from_dict(all_records, orient='columns')

end = time.time()
print(f"Loading Darshan trace took {end-start} seconds.")