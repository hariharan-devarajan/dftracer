from time import sleep
import os
import threading
from multiprocessing import get_context
import numpy as np
import PIL.Image as im

from dlio_profiler.logger import dlio_logger, fn_interceptor

cwd = os.getcwd()
log_file = os.getenv("LOG_FILE", f"{cwd}/test_py-app.pwf")
log_inst = dlio_logger.initialize_log(logfile=None, data_dir=None, process_id=-1)
dlio_log = fn_interceptor("COMPUTE")


@dlio_log.log
def log_events(index):
    sleep(1)


def custom_events():
    args = {
        "epoch": "1",
        "index": "1",
    }
    start = log_inst.get_time()
    sleep(1)
    end = log_inst.get_time()
    log_inst.log_event("test", "cat2", start, end - start, string_args=args)
    for i in dlio_log.iter(range(2)):
        sleep(1)


def posix_calls(val):
    index, is_spawn = val
    path = f"{cwd}/data/demofile{index}.txt"
    f = open(path, "w+")
    f.write("Now the file has more content!")
    f.close()
    if is_spawn:
        print(f"Calling spawn on {index} with pid {os.getpid()}")
        log_inst.finalize()
    else:
        print(f"Not calling spawn on {index} with pid {os.getpid()}")


def npz_calls(index):
    # print(f"{cwd}/data/demofile2.npz")
    path = f"{cwd}/data/demofile{index}.npz"
    if os.path.exists(path):
        os.remove(path)
    records = np.random.randint(255, size=(8, 8, 1024), dtype=np.uint8)
    record_labels = [0] * 1024
    np.savez(path, x=records, y=record_labels)


def jpeg_calls(index):
    records = np.random.randint(255, size=(1024, 1024), dtype=np.uint8)
    img = im.fromarray(records)
    out_path_spec = f"{cwd}/data/test.jpeg"
    img.save(out_path_spec, format='JPEG', bits=8)
    with open(out_path_spec, "rb") as f:
        image = im.open(f)
        out_records = np.asarray(image)
    # image = im.open(out_path_spec)


def init():
    """This function is called when new processes start."""
    print(f'Initializing process {os.getpid()}')


def main():
    posix_calls((20,False))
    t1 = threading.Thread(target=posix_calls, args=((10, False),))
    custom_events()
    t2 = threading.Thread(target=npz_calls, args=(1,))
    t3 = threading.Thread(target=jpeg_calls, args=(2,))
    t4 = threading.Thread(target=log_events, args=(3,))
    # starting thread 1
    t1.start()
    t2.start()
    t3.start()
    t4.start()

    t1.join()
    t2.join()
    t3.join()
    t4.join()
    index = 4
    with get_context('fork').Pool(1, initializer=init) as pool:
        pool.map(posix_calls, ((index, False),))
    index = index + 1

    with get_context('spawn').Pool(1, initializer=init) as pool:
        pool.map(posix_calls, ((index, True),))
    index = index + 1

    log_inst.finalize()


if __name__ == "__main__":
    main()
