[![DLIO Profiler Build and Test](https://github.com/hariharan-devarajan/dlio-profiler/actions/workflows/ci.yml/badge.svg)](https://github.com/hariharan-devarajan/dlio-profiler/actions/workflows/ci.yml)
[![Coverage Status](https://coveralls.io/repos/github/hariharan-devarajan/dlio-profiler/badge.svg?branch=feature/apis)](https://coveralls.io/github/hariharan-devarajan/dlio-profiler?branch=dev)
[![Documentation Status](https://readthedocs.org/projects/dlio-profiler/badge/?version=latest)](https://dlio-profiler.readthedocs.io/en/latest/?badge=latest)

# dlio-profiler
A low-level profiler for capture I/O calls from deep learning applications.

Requirements
1. Python > 3.8


## Build DLIO Profiler with pip

Users can easily install DLIO profiler using pip. This is the way most python packages are installed.
This method would work for both native python environments and conda environments.

### From source

```bash
    git clone git@github.com:hariharan-devarajan/dlio-profiler.git
    cd dlio-profiler
    # You can skip this for installing the dev branch.
    # for latest stable version use master branch.
    git checkout tags/<Release> -b <Release>
    pip install .
```

### From Github

```bash
DLP_VERSION=dev
pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git@${DLP_VERSION}
```

For more build instructions check [here](https://dlio-profiler.readthedocs.io/en/latest/build.html)

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

