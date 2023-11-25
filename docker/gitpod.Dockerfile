FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y gcc g++ python3.10 python3.10-venv \
    python3-pip openmpi-bin openmpi-common \
    libopenmpi-dev git cmake default-jre jq

RUN mkdir -p /workspace/venv
RUN python3.10 -m venv /workspace/venv

# Add contents of the current directory to /workspace/dlio in the container
ADD . /workspace/dlp

WORKDIR /workspace/dlp
RUN source /workspace/venv/bin/activate
RUN python -m pip install -e .
