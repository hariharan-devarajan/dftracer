#!/bin/bash
set -x
source $HOME/.dlio_profiler/configuration.sh
export PYTHONPATH=${DLIO_PROFILER_APP}:${PYTHONPATH}
# This can be set using env variable or arguments to script.
DLIO_PROFILER_DASK_CONF_NAME=${DLIO_PROFILER_APP}/dlp_analyzer/dask/conf/corona.yaml

# This is start of every script.
source ${DLIO_PROFILER_APP}/dlp_analyzer/dask/scripts/utils.sh
eval $(parse_yaml $DLIO_PROFILER_DASK_CONF_NAME DLIO_PROFILER_)


dask scheduler --scheduler-file ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler.json --port ${DLIO_PROFILER_CONFIG_DASK_SCHEDULER_PORT} > ${DLIO_PROFILER_CONFIG_LOG_DIR}/scheduler.log 2>&1 &
scheduler_pid=$!
echo $scheduler_pid > ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler.pid

${DLIO_PROFILER_CONFIG_JOB_ALLOC_CMD} ${DLIO_PROFILER_CONFIG_SCRIPT_DIR}/start_dask_worker.sh ${DLIO_PROFILER_DASK_CONF_NAME}
