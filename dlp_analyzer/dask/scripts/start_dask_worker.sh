#!/bin/bash

DLIO_PROFILER_DASK_CONF_NAME=$1

source $HOME/.dlio_profiler/configuration.sh
export PYTHONPATH=${DLIO_PROFILER_APP}:${PYTHONPATH}

# This is start of every script.
source ${DLIO_PROFILER_APP}/dlp_analyzer/dask/scripts/utils.sh
eval $(parse_yaml $DLIO_PROFILER_DASK_CONF_NAME DLIO_PROFILER_)
DLIO_PROFILER_JOB_ID=${!DLIO_PROFILER_JOB_ENV_ID}
source ${DLIO_PROFILER_ENV}/bin/activate
echo "Activated Env"
while :
do
${DLIO_PROFILER_WORKER_CMD} dask worker --scheduler-file ${DLIO_PROFILER_CONFIG_RUN_DIR}/scheduler_${USER}.json \
          --local-directory ${DLIO_PROFILER_WORKER_LOCAL_DIR} \
          --nworkers ${DLIO_PROFILER_WORKER_PER_CORE} --nthreads ${DLIO_PROFILER_WORKER_THREADS} > ${DLIO_PROFILER_CONFIG_LOG_DIR}/worker_${DLIO_PROFILER_JOB_ID}.log 2>&1
echo "Workers existed. Restarting in 1 second"
sleep 1
done
