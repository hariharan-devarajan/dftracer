========
Overview
========

DFTracer is I/O Profiling tool with the capability to doing application level as well as low-level I/O profiling.
Current I/O profilers such as Darshan and Recorder do not allow application level and I/O level profiling which makes it hard to analyze workloads such as AI and workflows.
To this end, we build a c++ profiler with binding to c and python. Additionally, we intercept low-level POSIX calls along with capturing Application level calls using APIs.

1. For build instructions see :doc:`build`
2. For api see :doc:`api`
3. For examples see :doc:`examples`