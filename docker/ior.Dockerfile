FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip openmpi-bin openmpi-common libopenmpi-dev
RUN apt-get install -y git cmake
RUN pip install git+https://github.com/hariharan-devarajan/dftracer.git
RUN git clone https://github.com/hpc/ior.git /ior
RUN apt-get install -y automake pkg-config
RUN cd /ior && ./bootstrap && ./configure && make -j
RUN mkdir -p /ior/data /ior/logs
ENV DFTRACER_ENABLE=1
ENV DFTRACER_DATA_DIR=testfile
ENV DFTRACER_LOG_LEVEL=INFO
ENV DFTRACER_INIT=PRELOAD
ENV DFTRACER_LOG_FILE=/ior/logs/trace
ENV DFTRACER_TRACE_COMPRESSION=1
ENV DFTRACER_INC_METADATA=1
RUN cd /ior/data && LD_PRELOAD=$(find /usr -name libdftracer_preload.so) mpirun -n 2 --allow-run-as-root ../src/ior -k -w -r -o testfile
RUN ls -l /ior/logs/
RUN gzip -d /ior/logs/*.pfw.gz
RUN cat /ior/logs/*.pfw