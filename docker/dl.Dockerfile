FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip openmpi-bin openmpi-common libopenmpi-dev
RUN apt-get install -y git cmake default-jre
RUN pip install "dlio_benchmark[dlio-profiler] git+https://github.com/argonne-lcf/dlio_benchmark.git"
RUN mkdir -p /dlio/data /dlio/output
ENV DLIO_PROFILER_ENABLE=1
ENV DLIO_PROFILER_LOG_LEVEL=INFO
ENV DLIO_PROFILER_INC_METADATA=1
RUN cd /dlio/data && mpirun -n 2 dlio_benchmark workload=resnet50 ++workload.workflow.generate_data=True ++workload.output.folder=/dlio/output
RUN gzip -d /dlio/output/.trace*.pfw.gz
RUN cat /dlio/output/.trace*.pfw