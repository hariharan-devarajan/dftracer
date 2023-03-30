# dlio-profiler
A low-level profiler for capture I/O calls from deep learning applications.


create a virtual env for your python package where u will use dlio_profiler.
```
python3 -m venv ./venv
source venv/bin/activate
```
Build dlio profiler
```
git clone git@github.com:hariharan-devarajan/dlio-profiler.git
cd dlio-profiler
mkdir build
cmake -DCMAKE_INSTALL_PREFIX=../../venv ../
make install -j
```

Usage
```
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
```
