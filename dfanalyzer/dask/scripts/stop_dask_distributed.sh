#!/bin/bash
set -x
source $HOME/.dftracer/configuration.sh
export PYTHONPATH=${DFTRACER_APP}:${PYTHONPATH}
# This can be set using env variable or arguments to script.

hostname=`hostname`
DFTRACER_DASK_CONF_NAME="UNSET"
case $hostname in
  *"corona"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/corona.yaml
    ;;
  *"ruby"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/ruby.yaml
    ;;
  "quartz"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/quartz.yaml
    ;;
  "polaris"*)
    DFTRACER_DASK_CONF_NAME=${DFTRACER_APP}/dfanalyzer/dask/conf/polaris.yaml
    ;;
esac

if [[ "$DFTRACER_DASK_CONF_NAME" == "UNSET" ]]; then
  echo "UNSUPPORTED $hostname"
  exit 1
fi

# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)

kill -9 `cat ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.pid`
export DFTRACER_JOB_ID=`cat ${DFTRACER_CONFIG_RUN_DIR}/job_id_${USER}.pid`
$DFTRACER_SCHEDULER_KILL $DFTRACER_JOB_ID
$DFTRACER_WORKER_KILL $DFTRACER_JOB_ID

