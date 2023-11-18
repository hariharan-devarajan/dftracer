#!/bin/bash
set -x
source $HOME/.dlio_profiler/configuration.sh
export PYTHONPATH=${DLIO_PROFILER_APP}:${PYTHONPATH}
# This can be set using env variable or arguments to script.
DLIO_PROFILER_DASK_CONF_NAME=${DLIO_PROFILER_APP}/dlp_analyzer/dask/conf/ruby.yaml

# This is start of every script.
source ${DLIO_PROFILER_APP}/dlp_analyzer/dask/scripts/utils.sh
eval $(parse_yaml $DLIO_PROFILER_DASK_CONF_NAME DLIO_PROFILER_)

$DLIO_PROFILER_SCHEDULER_KILL
$DLIO_PROFILER_WORKER_KILL
