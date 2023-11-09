FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip openmpi-bin openmpi-common libopenmpi-dev
RUN apt-get install -y git cmake
RUN pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git
RUN git clone https://github.com/hpc/ior.git /ior
RUN apt-get install -y automake pkg-config
RUN cd /ior && ./bootstrap && ./configure && make -j
RUN mkdir -p /ior/data /ior/logs
ENV DLIO_PROFILER_ENABLE=1
ENV DLIO_PROFILER_DATA_DIR=testfile
ENV DLIO_PROFILER_LOG_LEVEL=INFO
ENV DLIO_PROFILER_INIT=PRELOAD
ENV DLIO_PROFILER_LOG_FILE=/ior/logs/trace
ENV DLIO_PROFILER_TRACE_COMPRESSION=1
ENV DLIO_PROFILER_INC_METADATA=1
RUN cd /ior/data && LD_PRELOAD=$(find /usr -name libdlio_profiler_preload.so) mpirun -n 2 --allow-run-as-root ../src/ior -k -w -r -o testfile
RUN ls -l /ior/logs/
RUN gzip -d /ior/logs/*.pfw.gz
RUN cat /ior/logs/*.pfw