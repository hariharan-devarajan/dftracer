#!/usr/bin/env python
import argparse
import os
import numpy as np
from PIL import Image
import h5py
from dlio_profiler.logger import dlio_logger, fn_interceptor as Profile
parser = argparse.ArgumentParser(
    prog='DLIO testing',
    description='What the program does',
    epilog='Text at the bottom of help')
parser.add_argument('--log_dir', default="./pfw_logs", type=str, help="The log directory to save to the tracing")
parser.add_argument("--data_dir", default="./data", type=str, help="The directory to save and load data")
parser.add_argument("--format", default="npz", type=str, help="format of the file")
parser.add_argument("--num_files", default=1, type=int, help="Number of files")
parser.add_argument("--niter", default=1, type=int, help="Number of iterations for the experiment")
parser.add_argument("--record_size", default=1048576, type=int, help="size of the record to be written to the file")
args = parser.parse_args()
os.makedirs(f"{args.log_dir}/{args.format}", exist_ok=True)
os.makedirs(f"{args.data_dir}/{args.format}", exist_ok=True)
dlp = Profile("dlio")

class IOHandler:
    def __init__(self, filename, format):
        self.format = format
        self.filename = filename

class IOReader(IOHandler):
    @dlp.log_init
    def __init__(self, filename, format):
        super().__init__(filename=filename, format=format)
        if self.format == "jpeg" or self.format == "png":
            self.data = np.asarray(Image.open(filename))
        if self.format == "npz":
            self.data = np.load(filename)
        if self.format == "hdf5":
            fd = h5py.File(filename, 'r')
            self.data = fd['x'][:] # type: ignore
            fd.close()

    def get(self):
        return self.data

class IOWriter(IOHandler):
    @dlp.log_init
    def __init__(self, filename, format, data):
        super().__init__(filename=filename, format=format)
        if self.format == "jpeg" or self.format == "png":
            im = Image.fromarray(data)
            #im.show()
            im.save(filename)
        if self.format == "npz":
            with open(filename, 'wb') as f:
                np.save(f, data)
        if self.format == "hdf5":
            fd = h5py.File(filename, 'w')
            fd.create_dataset("x", data=data)
            fd.close()


@dlp.log
def data_gen(data):
    for i in dlp.iter(range(args.num_files)):
        i = IOWriter(filename=f"{args.data_dir}/{args.format}/{i}-of-{args.num_files}.{args.format}", format=args.format, data=data)

@dlp.log
def read_data(epoch):
    for i in dlp.iter(range(args.num_files)):
        io = IOReader(filename=f"{args.data_dir}/{args.format}/{i}-of-{args.num_files}.{args.format}", format=args.format)
        d = io.get()

if __name__ == "__main__":
    # Writing data
    data = np.ones((args.record_size, 1), dtype=np.uint8)
    dlp_logger = dlio_logger.initialize_log(f"{args.log_dir}_{args.format}.pfw", data_dir=None, process_id=-1)
    data_gen(data=data)
    for n in range(args.niter):
        read_data(epoch=n)
    dlp_logger.finalize()
