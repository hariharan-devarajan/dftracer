===========================
Limitations
===========================


---------------------
Spawning of processes
---------------------

For certain system the spawning of processes create new processes but do not carry env variable.
In those cases the LD_PRELOAD or python module load would not load DLIO Profiler as they are removed by system and result in missing profiling info.

----------------
OS Compatibility
----------------

The profiler internally uses system calls such as ``getpid()`` and ``gettid()`` which are only implemented in Linux OS.

-------------------------
Write Buffering on Docker
-------------------------

Within Docker there is an issue with buffers which prevents I/O from flushing.
To enable logging, we need to reduce write buffer size by setting ``DLIO_PROFILER_WRITE_BUFFER_SIZE=10``.
This will disable buffering and everything gets flushed synchronously.
However, reducing buffer size would increase overhead of operation profiling.