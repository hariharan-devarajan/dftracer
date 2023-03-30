import dlio_profiler_py as logger
from time import sleep
import os
cwd = os.getcwd()
def custom_events():
    logger.start("test", "cat2")
    sleep(2)
    logger.stop()

def posix_calls1():
    f = open(f"{cwd}/data/demofile2.txt", "w+")
    f.write("Now the file has more content!")
    f.close()

def posix_calls2():
    f = open(f"{cwd}/data/demofile2.txt", "r")
    data = f.read()
    f.close()

posix_calls1()
custom_events()
posix_calls2()