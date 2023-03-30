import dlio_profiler_py as logger
from time import sleep
import os

cwd = os.getcwd()


def custom_events():
    args = {
        "epoch": 1,
        "index": 1,
    }
    start = logger.get_time()
    sleep(2)
    end = logger.get_time()
    logger.log_event("test", "cat2", start, end - start, int_args=args)


def posix_calls1():
    print(f"{cwd}/data/demofile2.txt")
    f = open(f"{cwd}/data/demofile2.txt", "w+")
    f.write("Now the file has more content!")
    f.close()


def posix_calls2():
    print(f"{cwd}/data/demofile2.txt")
    f = open(f"{cwd}/data/demofile2.txt", "r")
    data = f.read()
    f.close()


logger.initialize(f"{cwd}/log.pwf", f"{cwd}/data")
posix_calls1()
custom_events()
posix_calls2()
logger.finalize()
