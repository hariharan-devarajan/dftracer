===========================
Limitations
===========================

For certain system the spawning of processes create new processes but do not carry env variable.
In those cases the LD_PRELOAD or python module load would not load DLIO Profiler as they are removed by system and result in missing profiling info.

----------------
OS Compatibility
----------------

The profiler internally uses system calls such as ``getpid()`` and ``gettid()`` which are only implemented in Linux OS.
