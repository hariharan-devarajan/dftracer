#!/bin/bash
set -x
source $HOME/.dlio_profiler/configuration.sh
export PYTHONPATH=${DLIO_PROFILER_APP}:${PYTHONPATH}

hostname=`hostname`
DLIO_PROFILER_DASK_CONF_NAME="UNSET"
case $hostname in
  *"corona"*)
    DLIO_PROFILER_DASK_CONF_NAME=${DLIO_PROFILER_APP}/dlp_analyzer/dask/conf/corona.yaml
    ;;
  *"ruby"*)
    DLIO_PROFILER_DASK_CONF_NAME=${DLIO_PROFILER_APP}/dlp_analyzer/dask/conf/ruby.yaml
    ;;
esac

if [[ "$DLIO_PROFILER_DASK_CONF_NAME" == "UNSET" ]]; then
  echo "UNSUPPORTED $hostname"
  exit 1
fi

# This can be set using env variable or arguments to script.


# This is start of every script.
source ${DLIO_PROFILER_APP}/dlp_analyzer/dask/scripts/utils.sh
eval $(parse_yaml $DLIO_PROFILER_DASK_CONF_NAME DLIO_PROFILER_)


dask scheduler --scheduler-file ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler.json --port ${DLIO_PROFILER_SCHEDULER_PORT} > ${DLIO_PROFILER_CONFIG_LOG_DIR}/scheduler.log 2>&1 &
scheduler_pid=$!
echo $scheduler_pid > ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler.pid

${DLIO_PROFILER_SCHEDULER_CMD} ${DLIO_PROFILER_CONFIG_SCRIPT_DIR}/start_dask_worker.sh ${DLIO_PROFILER_DASK_CONF_NAME}