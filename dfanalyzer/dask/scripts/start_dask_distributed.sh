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

# This can be set using env variable or arguments to script.


# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)

source ${DFTRACER_ENV}/bin/activate

rm -rf ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json

${DFTRACER_DASK_SCHEDULER} --scheduler-file ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json --port ${DFTRACER_SCHEDULER_PORT} > ${DFTRACER_CONFIG_LOG_DIR}/scheduler_${USER}.log 2>&1 &
scheduler_pid=$!
echo $scheduler_pid > ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.pid

file=${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json
timeout=30  # seconds to wait for timeout
SECONDS=0   # initialize bash's builtin counter 

until [ -s "$file" ] || (( SECONDS >= timeout )); do sleep 1; done


if test -f $file;
then
   echo "Scheduler with $scheduler_pid is running"
   # Do something knowing the pid exists, i.e. the process with $PID is running
else
   echo "Scheduler with $scheduler_pid failed. Check the ${DFTRACER_CONFIG_LOG_DIR}/scheduler_${USER}.log file"
   exit 1
fi

rm ${DFTRACER_CONFIG_RUN_DIR}/job_id_${USER}.pid

${DFTRACER_SCHEDULER_CMD} ${DFTRACER_CONFIG_SCRIPT_DIR}/start_dask_worker.sh ${DFTRACER_DASK_CONF_NAME} ${hostname}


