#!/bin/bash

DLIO_PROFILER_DASK_CONF_NAME=$1

source $HOME/.dlio_profiler/configuration.sh
export PYTHONPATH=${DLIO_PROFILER_APP}:${PYTHONPATH}

# This is start of every script.
source ${DLIO_PROFILER_APP}/dlp_analyzer/dask/scripts/utils.sh
eval $(parse_yaml $DLIO_PROFILER_DASK_CONF_NAME DLIO_PROFILER_)
DLIO_PROFILER_JOB_ID=${!DLIO_PROFILER_CONFIG_JOB_ID}

${DLIO_PROFILER_CONFIG_EXEC_CMD} dask worker --scheduler-file ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler.json \
          --local-directory ${DLIO_PROFILER_CONFIG_WORKER_LOCAL_DIR} \
          --nworkers ${DLIO_PROFILER_CONFIG_WORKER_PER_NODE} > ${DLIO_PROFILER_CONFIG_LOG_DIR}/worker_${DLIO_PROFILER_JOB_ID}.log 2>&1 &
worker_pid=$!
echo $worker_pid > ${DLIO_PROFILER_CONFIG_RUN_DIR}/worker_pid.pid