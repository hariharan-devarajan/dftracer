FROM ubuntu:latest
RUN apt-get update && apt-get install -y python3 python3-pip mpich libhwloc-dev git cmake default-jre jq
RUN pip install "dlio_benchmark @ git+https://github.com/argonne-lcf/dlio_benchmark.git"
RUN mkdir -p /dlio/data /dlio/output
RUN mpirun -np 4 dlio_benchmark workload=resnet50 ++workload.dataset.data_folder=/dlio/data ++workload.output.folder=/dlio/output ++workload.workflow.generate_data=True ++workload.workflow.train=False
ENV DLIO_PROFILER_LOG_LEVEL=ERROR
ENV DLIO_PROFILER_INC_METADATA=1
ENV DLIO_PROFILER_ENABLE=1
ENV RDMAV_FORK_SAFE=1
RUN mpirun -np 4 dlio_benchmark workload=resnet50 ++workload.dataset.data_folder=/dlio/data ++workload.output.folder=/dlio/output
RUN ls -al /dlio/output/
ENV filename=/dlio/output/trace*.pfw
RUN cat $filename | grep -v "\["   | awk '{$1=$1;print}' > /dlio/output/combined.json
RUN jq '.' /dlio/output/combined.json > /dev/null
RUN echo "[" >  /dlio/output/combined.pfw
RUN cat $filename | grep -v "\["   | awk '{$1=$1;print}' | jq -R "fromjson? | . " -c >> /dlio/output/combined.pfw
RUN cat /dlio/output/combined.pfw | wc -l