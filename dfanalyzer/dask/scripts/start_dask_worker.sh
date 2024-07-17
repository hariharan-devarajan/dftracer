#!/bin/bash

set -x

DFTRACER_DASK_CONF_NAME=$1
DFTRACER_SCHEDULER_HOSTNAME=$2

source $HOME/.dftracer/configuration.sh
export PYTHONPATH=${DFTRACER_APP}:${PYTHONPATH}

# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)
DFTRACER_JOB_ID=${!DFTRACER_JOB_ENV_ID}

echo -n $DFTRACER_JOB_ID > ${DFTRACER_CONFIG_RUN_DIR}/job_id_${USER}.pid

source ${DFTRACER_ENV}/bin/activate
echo "Activated Env"

if [ "x${DFTRACER_WORKER_CONNECTION_STRING}" != "x" ]; then
    dask_scheduler_conn=${DFTRACER_WORKER_CONNECTION_STRING}
else
    dask_scheduler_conn=--scheduler-file ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json
fi

while :
do
${DFTRACER_WORKER_CMD} ${DFTRACER_DASK_WORKER} ${dask_scheduler_conn} \
          --local-directory ${DFTRACER_WORKER_LOCAL_DIR} \
          --nworkers ${DFTRACER_WORKER_PER_CORE} --nthreads ${DFTRACER_WORKER_THREADS} > ${DFTRACER_CONFIG_LOG_DIR}/worker_${DFTRACER_JOB_ID}.log 2>&1
echo "Workers existed. Restarting in 1 second"
sleep 1
done

