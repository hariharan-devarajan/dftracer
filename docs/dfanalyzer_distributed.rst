===================================
Running DFAnalyzer with Dask Distributed
===================================

-----------------------------------
Getting Started
-----------------------------------

1. Create a Python virtual environment (Python version>3.7).
2. Source the Python virtual environment.
3. Git clone the GitHub repo to get the source code of DFTracer.
4. Navigate into :code:`/path/to/dftracer/dfanalyzer/examples/dfanalyzer`.
5. Build DFTracer as recommended in :doc:`build`.
6. Get all of the requirements as follows in the terminal:

.. code-block:: bash

   pip install -r requirements.txt

7. Create a `.yaml` file in :code:`/path/to/dftracer/dfanalyzer/dask/conf` if this is a new system. Please refer to :doc:`dfanalyzer_conf`.

-----------------------------------
Starting a Dask Distributed Cluster
-----------------------------------

In the terminal:

.. code-block:: bash

   cd /path/to/dftracer/dfanalyzer/dask/conf
   ./install_dask_env.sh 

This will create the `configuration.yaml` in :code:`~/.dftracer`. Update the application and environment path in `configuration.yaml`. You may need to create `run_dir` and `logs` folders if they aren't there already.

.. code-block:: bash

   cd /path/to/dftracer/dfanalyzer/dask/
   # if logs folder is not present
   mkdir logs
   # if run_dir is not present
   mkdir run_dir
   install 
   ./scripts/start_dask_distributed.sh

.. note::

   Wait for several seconds as this script will reserve the compute nodes for you using the job scheduler.

.. note::

   Please check the log file :code:`/path/to/dftracer/dfanalyzer/dask/logs/worker_<jobid>.log` for any issues with running the workers on the compute nodes.

.. warning::

    For errors related to port usage, please check if you already have any Dask distributed instances running. You can do so by checking the jobs already running in your scheduler queue or by running the following command in the terminal:

    .. code-block:: bash

        ps -aef | grep dask
    
    Then kill those jobs/processes using :code:`kill -9 <pid>`. You may also need to change the port number in the `.yaml` files located at `/path/to/dftracer/dfanalyzer/dask/conf`. For more details about these configurations refer to :doc:`here <dfanalyzer_conf>`.


-----------------------------------
Use DFAnalyzer
-----------------------------------

To use the Jupyter notebook of DFAnalyzer, navigate to :code:`/path/to/dftracer/examples` and find the `dfanalyzer_distributed.ipynb`.

----------------------------------------
Acessing the Dask Dashboard
----------------------------------------

It is recommended to run the notebook inside VSCode because it supports port forwarding natively. In VSCode, navigate to the bottom bar (where the terminal is), and click on the :code:`PORTS` tab. Click :code:`Forward Port` to add a new port and type the port that was used when :code:`setup_dask_cluster()` was run in your `dfanalyzer.ipynb` notebook. Connect to `http://localhost:PORT <http://localhost:PORT>`_ to see the :code:`Dask` scheduler monitoring.

----------------------------------------
Stopping Dask Distributed Workers
----------------------------------------

.. code-block:: bash

   cd /path/to/dftracer/dfanalyzer/dask/scripts
   ./stop_dask_distributed.sh

.. note::

   Wait for several seconds as this script will terminate the workers and deallocate the compute nodes.
