#!/bin/bash
set -x
source $HOME/.dftracer/configuration.sh
export PYTHONPATH=${DFTRACER_APP}:${PYTHONPATH}

hostname=`hostname`
DFTRACER_DASK_CONF_NAME="UNSET"
case $hostname in
  *"corona"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/corona.yaml
    ;;
  *"ruby"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/ruby.yaml
    ;;
esac

if [[ "$DFTRACER_DASK_CONF_NAME" == "UNSET" ]]; then
  echo "UNSUPPORTED $hostname"
  exit 1
fi

# This can be set using env variable or arguments to script.


# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)

source ${DFTRACER_ENV}/bin/activate

dask scheduler --scheduler-file ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json --port ${DFTRACER_SCHEDULER_PORT} > ${DFTRACER_CONFIG_LOG_DIR}/scheduler_${USER}.log 2>&1 &
scheduler_pid=$!
echo $scheduler_pid > ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.pid

${DFTRACER_SCHEDULER_CMD} ${DFTRACER_CONFIG_SCRIPT_DIR}/start_dask_worker.sh ${DFTRACER_DASK_CONF_NAME}
