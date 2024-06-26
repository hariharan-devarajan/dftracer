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

import otf2
from otf2.events import *

<<<<<<< HEAD
=======

>>>>>>> d419e7e (Paper/ad ae (#100))
logging.basicConfig(filename='score-p_main.log', encoding='utf-8', level=logging.DEBUG)

def get_json(location, start, end):
    d = {}
    #print(location, start)
    d["name"] = start.region.name
    d["cat"] = start.region.region_role
    d["ts"] = start.time
    d["dur"] = end.time - start.time
    return d
def get_json_one(location, start):
    d = {}
    #print(location.group, start)
    if hasattr(start, 'region'):
        d["name"] = start.region.name
        d["cat"] = start.region.region_role
    else:
        d["name"] = start.__class__
        d["cat"] = "Program"        
    d["ts"] = start.time
    d["dur"] = 0
    d["tid"] = location.name
    d["pid"] = location.group.name
    return d


def read_trace(trace_name):
    map_events = {}
    count = 0
    with otf2.reader.open(trace_name) as trace:
        #print("Read {} string definitions".format(len(trace.definitions.strings)))
        for location, event in trace.events:
            if isinstance(event, Enter):
                unique_id = (location, event.region)
                map_events[unique_id] = [event]
                #print(f"Encountered enter event into {event.region} on location {location.group} at {event.attributes}")
            elif isinstance(event, Leave):
                unique_id = (location, event.region)
                if unique_id in map_events:
                    map_events[unique_id].append(event)
                else:
                    map_events[unique_id] = [event]
                #print(f"Encountered enter event int")
                if len(map_events[unique_id]) == 2:
                    yield dict(**get_json(location = location, start = map_events[unique_id][0], end = map_events[unique_id][1]))
                elif len(map_events[unique_id]) == 1:
                    yield dict(**get_json_one(location = location, start = map_events[unique_id][0]))
                del map_events[unique_id]
                #print(f"Encountered leave event for {event.region} on location {location} at {event}")
            else:
                yield dict(**get_json_one(location = location, start = event))
                #print(f"Encountered event on location {location} at {event}")
            count = count + 1
            if count % 1000 == 0:
                print(f"Done {count} in {time.time() - start}", end="\r")

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
print(f"Loading Score-P trace took {end-start} seconds.")