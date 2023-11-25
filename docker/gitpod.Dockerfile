# You can find the new timestamped tags here: https://hub.docker.com/r/gitpod/workspace-full/tags
FROM gitpod/workspace-full:latest

# Install custom tools, runtime, etc.
RUN apt-get update
RUN apt-get install -y gcc g++ python3.10 python3.10-venv \
    python3-pip openmpi-bin openmpi-common \
    libopenmpi-dev git cmake default-jre jq
SHELL ["/bin/bash", "-c"]
RUN mkdir -p /workspace/venv
RUN python3.10 -m venv /workspace/venv

# Add contents of the current directory to /workspace/dlio in the container
ADD . /workspace/dlp

WORKDIR /workspace/dlp
