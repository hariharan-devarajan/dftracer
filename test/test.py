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


def posix_calls1(index):
    path=f"{cwd}/data/demofile{index}.txt"
    f = open(path, "w+")
    f.write("Now the file has more content!")
    f.close()

import numpy as np

def posix_calls2(index):
    #print(f"{cwd}/data/demofile2.npz")
    path = f"{cwd}/data/demofile{index}.npz"
    if os.path.exists(path):
        os.remove(path)
    records = np.random.randint(255, size=(8, 8, 1024), dtype=np.uint8)
    record_labels = [0] * 1024
    np.savez(path, x=records, y=record_labels)

import threading

logger.initialize(f"{cwd}/log.pwf", f"{cwd}/data")
t1 = threading.Thread(target=posix_calls1, args=(10,))
custom_events()
t2 = threading.Thread(target=posix_calls2, args=(1,))
t3 = threading.Thread(target=posix_calls2, args=(2,))
# starting thread 1
t1.start()
t2.start()
t3.start()

t1.join()
t2.join()
t3.join()

logger.finalize()
