[![DLIO Profiler Build and Test](https://github.com/hariharan-devarajan/dlio-profiler/actions/workflows/ci.yml/badge.svg)](https://github.com/hariharan-devarajan/dlio-profiler/actions/workflows/ci.yml)
[![Coverage Status](https://coveralls.io/repos/github/hariharan-devarajan/dlio-profiler/badge.svg?branch=feature/apis)](https://coveralls.io/github/hariharan-devarajan/dlio-profiler?branch=dev)

# dlio-profiler
A low-level profiler for capture I/O calls from deep learning applications.

Requirements
1. Python > 3.8
2. spack

Using spack dlio-profiler
```
spack env create -d ./spack-env
spack env activate -p ./spack-env
spack add py-dlio-profiler@0.0.1
spack install
```

create a virtual env for your python package where u will use dlio_profiler.
```
python3 -m venv ./venv
source venv/bin/activate
pip install .
```
install in local user
```
export DLIO_LOGGER_USER=1
pip install .
```
install directly from github
```
pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git
```
Build dlio profiler through cmake
```
cd dlio-profiler
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../venv ../
make install -j
```

Usage

```
    from dlio_profiler.logger import dlio_logger, fn_interceptor
    log_inst = dlio_logger.initialize_log(logfile=None, data_dir=None, process_id=-1)
    dlio_log = fn_interceptor("COMPUTE")

    # Example of using function decorators
    @dlio_log.log
    def log_events(index):
        sleep(1)

    # Example of function spawning and implicit I/O calls
    def posix_calls(val):
        index, is_spawn = val
        path = f"{cwd}/data/demofile{index}.txt"
        f = open(path, "w+")
        f.write("Now the file has more content!")
        f.close()
        if is_spawn:
            print(f"Calling spawn on {index} with pid {os.getpid()}")
            log_inst.finalize() # This need to be called to correctly finalize DLIO Profiler.
        else:
            print(f"Not calling spawn on {index} with pid {os.getpid()}")

    # NPZ calls internally calls POSIX calls.
    def npz_calls(index):
        # print(f"{cwd}/data/demofile2.npz")
        path = f"{cwd}/data/demofile{index}.npz"
        if os.path.exists(path):
            os.remove(path)
        records = np.random.randint(255, size=(8, 8, 1024), dtype=np.uint8)
        record_labels = [0] * 1024
        np.savez(path, x=records, y=record_labels)

    def main():
        log_events(0)
        npz_calls(1)
        with get_context('spawn').Pool(1, initializer=init) as pool:
            pool.map(posix_calls, ((2, True),))
        log_inst.finalize()


    if __name__ == "__main__":
        main()

```

For this example, as the DLIO_PROFILER_CPP_INIT do not pass log file or data dir, we need to set ``DLIO_PROFILER_LOG_FILE`` and ``DLIO_PROFILER_DATA_DIR``.
By default the DLIO Profiler mode is set to FUNCTION.
Example of running this configurations are:

```

    # the process id, app_name and .pfw will be appended by the profiler for each app and process.
    # name of final log file is ~/log_file-<APP_NAME>-<PID>.pfw
    DLIO_PROFILER_LOG_FILE=~/log_file
    # Colon separated paths for including for profiler
    DLIO_PROFILER_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset:$PWD/data
    # Enable profiler
    DLIO_PROFILER_ENABLE=1
```

For more example check [Examples](https://dlio-profiler.readthedocs.io/en/latest/examples.html).

