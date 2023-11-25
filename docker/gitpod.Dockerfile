FROM ubuntu:22.04

RUN apt-get update 
RUN apt-get install -y gcc g++ python3.11 \
    python3-pip openmpi-bin openmpi-common \
    libopenmpi-dev git cmake default-jre jq

# Add contents of the current directory to /workspace/dlio in the container
ADD . /workspace/dlp

WORKDIR /workspace/dlp
RUN python3.11 -m pip install -e .