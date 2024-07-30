#!/bin/bash

LOG_DIR=$1
OUTPUT_DIR=$2
LINES=$3
APP_NAME=$4

if [ "x${LINES}" == "x" ]; then
  echo "LINES not set"
  echo "Usage dftracer_compact <DFTRACER_LOG_DIR> <OUTPUT_DIR> <LINES> <APP_NAME>"
  exit 0
fi

if [ "x${APP_NAME}" == "x" ]; then
  echo "APP_NAME not set"
  echo "Usage dftracer_compact <DFTRACER_LOG_DIR> <OUTPUT_DIR> <LINES> <APP_NAME>"
  exit 0
fi

if [ ! -d ${LOG_DIR} ]; then
  echo "DFTracer Log Directory not found."
  echo "Usage dftracer_compact <DFTRACER_LOG_DIR> <OUTPUT_DIR> <LINES> <APP_NAME>"
  exit 0
fi

if [ -d ${OUTPUT_DIR} ]; then
  echo "Output directory should not exist"
  echo "Usage dftracer_compact <DFTRACER_LOG_DIR> <OUTPUT_DIR> <LINES> <APP_NAME>"
  exit 0
fi

mkdir -p ${OUTPUT_DIR}

ls ${LOG_DIR}/*.pfw | grep -v "meta" | xargs cat | grep -v "^\[" | jq -c '.' > ${OUTPUT_DIR}/temp.combined.pfw
ls ${LOG_DIR}/*meta.pfw | xargs cat | grep -v "^\[" | jq -c '.' > ${OUTPUT_DIR}/${APP_NAME}-meta.pfw

pushd ${OUTPUT_DIR}

split -l ${LINES} --numeric-suffixes  --additional-suffix=.pfw ${OUTPUT_DIR}/temp.combined.pfw ${APP_NAME}-
gzip *.pfw

popd

