#!/bin/bash

DFTRACER_DASK_CONF_NAME=$1

source $HOME/.dftracer/configuration.sh
export PYTHONPATH=${DFTRACER_APP}:${PYTHONPATH}

# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)
DFTRACER_JOB_ID=${!DFTRACER_JOB_ENV_ID}
source ${DFTRACER_ENV}/bin/activate
echo "Activated Env"
while :
do
${DFTRACER_WORKER_CMD} dask worker --scheduler-file ${DFTRACER_CONFIG_RUN_DIR}/scheduler_${USER}.json \
          --local-directory ${DFTRACER_WORKER_LOCAL_DIR} \
          --nworkers ${DFTRACER_WORKER_PER_CORE} --nthreads ${DFTRACER_WORKER_THREADS} > ${DFTRACER_CONFIG_LOG_DIR}/worker_${DFTRACER_JOB_ID}.log 2>&1
echo "Workers existed. Restarting in 1 second"
sleep 1
done
