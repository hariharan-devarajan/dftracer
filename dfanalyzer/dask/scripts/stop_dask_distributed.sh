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
esac

if [[ "$DFTRACER_DASK_CONF_NAME" == "UNSET" ]]; then
echo "UNSUPPORTED $hostname"
exit 1
fi

# This is start of every script.
source ${DFTRACER_APP}/dfanalyzer/dask/scripts/utils.sh
eval $(parse_yaml $DFTRACER_DASK_CONF_NAME DFTRACER_)

$DFTRACER_SCHEDULER_KILL
$DFTRACER_WORKER_KILL


