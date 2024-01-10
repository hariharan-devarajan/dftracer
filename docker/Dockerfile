FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip openmpi-bin openmpi-common libopenmpi-dev
RUN apt-get install -y git cmake
RUN pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git
RUN pip install --disable-pip-version-check \
                                      --no-cache-dir \
                                      --no-compile \
                                      --upgrade \
                      mpi4py numpy h5py pandas psutil hydra-core==1.2.0 \
                      tensorflow torch torchaudio torchvision nvidia-dali-cuda110
RUN pip install 'hydra-core @ git+https://github.com/facebookresearch/hydra.git@v1.3.2#egg=hydra-core'
RUN mkdir -p /dlio/data /dlio/output /dlio/logs
RUN git clone https://github.com/argonne-lcf/dlio_benchmark.git /dlio/source
ENV DLIO_PROFILER_ENABLE=1
ENV DLIO_PROFILER_LOG_LEVEL=INFO
ENV DLIO_PROFILER_INC_METADATA=1
RUN cd /dlio/data && mpirun -n 2 python /dlio/source/dlio_benchmark/main.py workload=resnet50 ++workload.workflow.generate_data=True ++workload.output.folder=/dlio/output
RUN gzip -d /dlio/output/.trace*.pfw.gz
RUN cat /dlio/output/.trace*.pfw