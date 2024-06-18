import sys
import logging
import os
from mpi4py import MPI
from time import time
from dftracer.logger import dftracer, dft_fn

log_inst = dftracer.initialize_log(logfile=None, data_dir=None, process_id=-1)

class Timer:
    def __init__(self):
        self.elapsed_time = 0
        self.start = 0
    def resume_time(self):
        self.start = time()

    def pause_time(self):
        self.elapsed_time += time() - self.start


def main(argc, argv):
    if argc < 5:
        raise Exception("python overhead.py <TEST_DIR> <NUM_OPS> <TRANSFER_SIZE>")
    logging.info(f"{argv}")
    dir = argv[2]
    num_operations = int(argv[3])
    transfer_size = int(argv[4])
    logging.basicConfig(filename=f'{os.getcwd()}/overhead_python_{MPI.COMM_WORLD.rank}.log', encoding='utf-8', level=logging.DEBUG)
    path = f"{dir}/file_{MPI.COMM_WORLD.rank}-{MPI.COMM_WORLD.size}.bat"
    operation_time = Timer()
    operation_time.resume_time()
    f = open(path, "w+")
    operation_time.pause_time()
    buffer = 'w' * transfer_size
    for i in range(num_operations):
        operation_time.resume_time()
        f.write(buffer)
        operation_time.pause_time()

    operation_time.resume_time()
    f.close()
    operation_time.pause_time()
    total_time = 0.0
    total_time = MPI.COMM_WORLD.allreduce(operation_time.elapsed_time, op=MPI.SUM)
    if MPI.COMM_WORLD.rank == 0:
        print(f"[DFTRACER PRINT],{MPI.COMM_WORLD.size},{num_operations},{transfer_size},{total_time}")
    MPI.COMM_WORLD.barrier()
    os.remove(path)
    log_inst.finalize()

if __name__ == "__main__":
    argc = len(sys.argv)
    argv = sys.argv
    main(argc, argv)