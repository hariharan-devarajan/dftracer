======================================
Running Dask Distributed in a new system
======================================

This section describes how to configure DFAnalyzer to run on your cluster.

----------

Make sure you have already completed the necessary steps to build the :code:`dfanalyzer`. See the :doc:`build` documentation for details.

----------------------------------------
Initializing Dask Configurations
----------------------------------------

.. code-block:: bash

   cd <dftracer>/dfanalyzer/dask/conf
   ./install_dask_env.sh

.. note::
   
   This will create a new directory :code:`$HOME/.dftracer/` with files: :code:`$HOME/.dftracer/configuration.sh` and :code:`$HOME/.dftracer/configuration.yaml`

----------------------------------------
Editing :code:`$HOME/.dftracer/configuration.yaml`
----------------------------------------

.. code-block:: bash

   cd $HOME/.dftracer
   <EDITOR> configuration.yaml

By default, :code:`$HOME/.dftracer/configuration.yaml` will contain this entry:

.. code-block:: yaml

   app: /usr/WS2/haridev/dftracer
   env: ${DFTRACER_APP}/venv

Please modify the :code:`app` to your cloned :code:`<dftracer>` directory and :code:`env` to the Python virtual environment that you used to install the :code:`dfanalyzer` code. Refer to the :doc:`build` documentation for details.

----------------------------------------
Editing Dask Configurations depending on your System
----------------------------------------

In the `<dftracer>/dfanalyzer/dask/conf/` folder create a new `.yaml` file for the system you want to use. The `.yaml` file should consist of the following fields:

- config: this fiels contains locations for the directories containing files for the dask distributed cluster.
   - script_dir: the scripts to run dask distributed
   - conf_dir: the `.yaml` configuration files
   - run_dir: the folder which will contain the :code:`scheduler_{$USER}.pid` and :code:`scheduler_{$USER}.json` file used to store information for the scheduler.
   - log_dir: the folder which will contain the logs for the dask distributed scheduler and workers.
- job: information about the job you want to run to create the dask distributed cluster.
   - num_nodes: number of nodes which are going to be used to run the dask distributed cluster.
   - wall_time_min: time (in minutes) which the dask distributed cluster is going to run for.
   - env_id: the name of the job which will run.
   - queue: the queue which the job will run for.
- scheduler: information used to run the scheduler of the dask distributed cluster.
   - cmd: command used to run the scheduler. Depending on the system you are using you might need to use FLUX, SLURM or other scheduler.
   Examples can look like this: :code:`srun -N ${DFTRACER_JOB_NUM_NODES} -t ${DFTRACER_JOB_WALL_TIME_MIN}` for SLURM scheduler or :code:`flux run -N ${DFTRACER_JOB_NUM_NODES} -t ${DFTRACER_JOB_WALL_TIME_MIN}` for FLUX scheduler.
   - port: :code:`<port>`` used to run dask distributed.
   - kill: command used to kill the cluster.
   Examples can look like this: :code:`scancel ${SLURM_JOB_ID}` for SLURM scheduler or `flux cancel --all` for FLUX scheduler.
- worker: information used to run the workers of the dask distributed cluster.
   - ppn: processes per node for the dask distributed cluster.
   - cmd: command used to run the worker. Depending on the system you are using you might need to use FLUX, SLURM or other scheduler.
   Examples can look like this: :code:`srun -N ${DFTRACER_JOB_NUM_NODES} --ntasks-per-node=${DFTRACER_WORKER_PPN}` for SLURM scheduler or :code:`srun -N ${DFTRACER_JOB_NUM_NODES} --ntasks-per-node=${DFTRACER_WORKER_PPN}` for FLUX scheduler.
   - per_core: number of processes per code
   - threads: number of threads used.
   - local_dir: a location for a local directory used from dask to cache data frames. It can be set to local storage or shared memory.
   - kill: command used to kill the cluster.
   Examples can look like this: :code:`scancel ${SLURM_JOB_ID}` for SLURM scheduler or :code:`flux cancel --all` for FLUX scheduler.

.. code-block:: bash

   cd <dftracer>/dfanalyzer/dask/conf
   <EDITOR> <system>.yaml

Bellow is an example of a `.yaml` taht can used for LC Ruby:

.. code-block:: bash
   config:
      script_dir: ${DFTRACER_APP}/dfanalyzer/dask/scripts
      conf_dir: ${DFTRACER_APP}/dfanalyzer/dask/conf
      run_dir: ${DFTRACER_APP}/dfanalyzer/dask/run_dir
      log_dir: ${DFTRACER_APP}/dfanalyzer/dask/logs
   job:
      num_nodes: 1
      wall_time_min: 60
      env_id: SLURM_JOB_ID
   worker:
      ppn: 16
      cmd: srun -N ${DFTRACER_JOB_NUM_NODES} -t ${DFTRACER_JOB_WALL_TIME_MIN}
      per_core: 1
      threads: 1
      local_dir: /dev/shm/dask-workspace
      kill: scancel ${SLURM_JOB_ID}
   scheduler:
      cmd: srun -N ${DFTRACER_JOB_NUM_NODES} -t ${DFTRACER_JOB_WALL_TIME_MIN} --ntasks-per-node=${DFTRACER_WORKER_PPN}
      port: 12001
      kill: scancel ${SLURM_JOB_ID}

----------------------------------------
Run DFAnalyzer
----------------------------------------

Navigate to :code:`<dftracer>/examples/dfanalyzer/dfanalyzer_distributed.ipynb` and run your notebook.
