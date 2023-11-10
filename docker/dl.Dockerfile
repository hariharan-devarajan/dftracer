FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip openmpi-bin openmpi-common libopenmpi-dev git cmake default-jre jq
RUN pip install "dlio_benchmark @ git+https://github.com/argonne-lcf/dlio_benchmark.git"
RUN pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git
RUN mkdir -p /dlio/data /dlio/output
RUN mpirun -n 4 --allow-run-as-root dlio_benchmark workload=resnet50 ++workload.dataset.data_folder=/dlio/data ++workload.output.folder=/dlio/output ++workload.workflow.generate_data=True ++workload.workflow.train=False
ENV DLIO_PROFILER_ENABLE=1
ENV DLIO_PROFILER_LOG_LEVEL=ERROR
ENV DLIO_PROFILER_INC_METADATA=1
ENV DLIO_PROFILER_ENABLE=1
RUN mpirun -n 4 --allow-run-as-root dlio_benchmark workload=resnet50 ++workload.dataset.data_folder=/dlio/data ++workload.output.folder=/dlio/output
RUN ls -al /dlio/output/
ENV filename=/dlio/output/.trace*.pfw
RUN cat $filename | grep -v "\["   | awk '{$1=$1;print}' > /dlio/output/combined.json
RUN jq '.' /dlio/output/combined.json > /dev/null
RUN echo "[" >  /dlio/output/combined.pfw
RUN cat $filename | grep -v "\["   | awk '{$1=$1;print}' | jq -R "fromjson? | . " -c >> /dlio/output/combined.pfw
RUN cat /dlio/output/combined.pfw | wc -l