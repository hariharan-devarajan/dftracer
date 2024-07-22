===================================
Running DFAnalyzer
===================================

-----------------------------------
Getting Started
-----------------------------------

1. Create a Python virtual environment (Python version >3.7).
2. Source the Python virtual environment.
3. Git clone the GitHub repo to get the source code of DFTracer.
4. Navigate into /path/to/dftracer/dfanalyzer/examples/dfanalyzer.
5. Build DFTracer as recommended in :doc:`build`.
6. Get all of the requirements as follows in the terminal:

   .. code-block:: bash
      pip install -r requirements.txt

-----------------------------------
Starting a Dask Distributed Cluster
-----------------------------------

In the terminal:

.. code-block:: bash
   cd /path/to/dftracer/dfanalyzer/dask/conf
   install_dask_env.sh 

This will create the `configuration.yaml`` in `~/.dftracer`. Update the application and environment path in `configuration.yaml`.

.. code-block:: bash
   cd /path/to/dftracer/dfanalyzer/dask/
   mkdir logs
   mkdir run_dir
   install 
   ./scripts/start_dask_distributed.sh

-----------------------------------
Use DFAnalyzer
-----------------------------------

To use the Jupyter notebook of DFAnalyzer, navigate to /path/to/dftracer/examples and find the `dfanalyzer.ipynb`.

-----------------------------------
Ensure that Dask Distributed is running
-----------------------------------

To ensure that Dask distributed is running, you have to make sure that the Workers are up after running the 
"start_dask_distributed.sh" script. 
You can change the configurations such as the number of nodes and the processes per node using the `.yaml` files located 
at `/path/to/dftracer/dfanalyzer/dask/conf`. 
You can check if Dask distributed is running by forwarding the port that appears after running `setup_dask_cluster()` in the 
Jupyter notebook `dlp_analyzer.ipynb` and using localhost as the IP address.

.. note::

    For errors related to port usage, please check if you already have any Dask distributed instances running. You can do so by checking the 
    jobs already running in your scheduler queue or by running the following command in the terminal:

    .. code-block:: bash
        ps -aef | grep dask
    
    Then kill those jobs/processes. You may also need to change the port number in the `.yaml` files located at `/path/to/dftracer/dfanalyzer/dask/conf`.