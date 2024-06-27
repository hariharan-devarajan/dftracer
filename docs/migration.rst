================================================
Migration from DLIO Profiler to DFTracer
================================================

This section provides information on how to migrate from DLIO profiler to DFTracer.


------------------------------------------------
Inatallation and Python API changes
------------------------------------------------


------------------------------------------------
C and C++ Changes
------------------------------------------------


------------------------------------------------
Analyzer Changes
------------------------------------------------

Migration of the DLP Analyzer jupyter notebook to DFAnalyzer involves configuring the YAML for Dask and renaming the imports and function calls in jupyter notebook cells.

Dask Configuration:
**************************
1. ``cd`` to ``dftracer/dfanalyzer/dask/conf`` and run ``install_dask_env.sh``  to create configuration.yaml  in ``~/.dftracer``.
2. update the app and environment path in ``configuration.yaml``.

Jupyter Notebook Update:
**************************
1. update ``app_root`` variable by updating path of new ``configuration.yaml``.
2. replace ``dlp_analyzer`` with ``dfanalyzer`` and update the imports form ``dfanalyzer.main``

.. code-block:: python
   :linenos:

     ...
     import dfanalyzer
     from dfanalyzer.main import DFAnalyzer,get_dft_configuration,update_dft_configuration,setup_logging,setup_dask_cluster, reset_dask_cluster, get_dft_configuration
     ...

3. update the ``dask_run_dir`` to use dfanalyzer instead of dlp_analyzer.
4. rename update and get configuration functions by calling DFtracer equivalent functions.
.. code-block:: python
   :linenos:

     ...
     conf = update_dft_configuration(dask_scheduler=dask_scheduler, verbose=True, 
                                log_file=f"./dft_{os.getenv('USER')}.log", rebuild_index=False, time_approximate=False, 
                                host_pattern=r'lassen(\d+)', time_granularity=30e6, skip_hostname=True, conditions=condition_fn)
     conf = get_dft_configuration()
     ...


