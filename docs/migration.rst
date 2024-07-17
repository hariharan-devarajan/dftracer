================================================
Migration from DLIO Profiler to DFTracer
================================================

This section provides information to DLIO Profiler users on how to migrate their work from DLIO profiler to DFTracer.

------------------------------------------------
Installation
------------------------------------------------

------------------------------------------------
Application building migration
------------------------------------------------
To migrate your Makefile projects from using DLIO Profiler to DFTracer, you will need to update your compilation flags, specifically `CFLAGS` or `CXXFLAGS`, and `LDFLAGS`. 
Replace the DLIO Profiler flags with DFTracer flags as shown below:

.. code-block:: make
   :linenos:
   :caption: Modifying Makefile to use DFTracer

   # DLIO Profiler Flags (old)
   DLIO_CFLAGS = -I/usr/workspace/iopp/kogiou1/venvs/pegasus-env/lib/python3.9/site-packages/dlio_profiler/include
   DLIO_LDFLAGS = -L/usr/workspace/iopp/kogiou1/venvs/pegasus-env/lib/python3.9/site-packages/dlio_profiler/lib64 -ldlio_profiler
   CFLAGS += $(DLIO_CFLAGS)
   LIBS += $(DLIO_LDFLAGS)

   # Replace with DFTracer Flags (new)
   # Add DFTracer include and library paths
   DFTRACER_CFLAGS = -I/path/to/dftracer/include
   DFTRACER_LDFLAGS = -L/path/to/dftracer/lib64 -ldftracer

   # Append to existing CFLAGS and LDFLAGS
   CFLAGS += $(DFTRACER_CFLAGS)
   LDFLAGS += $(DFTRACER_LDFLAGS)

------------------------------------------------
C++ API Changes
------------------------------------------------

This section guides you through the necessary changes to migrate your application-level tracing for C++ projects from DLIO Profiler to DFTracer. The transition requires updating API names and includes directives to use the DFTracer's new API.
Please see `examples.rst` for more information on how to use DFTracer APIs.

Updating Includes
---------------------

Replace the old DLIO Profiler include header with the new DFTracer header. This change points your application to the new tracing API.

.. code-block:: cpp
  :linenos:

    // Old include
    #include <dlio_profiler/dlio_profiler.h>

    // Replace with new include
    #include <dftracer/dftracer.h>

Initializing
------------------------

Initialization now uses the DFTracer API, which can seamlessly integrate into your existing codebase where DLIO Profiler was previously initialized. 

.. code-block:: cpp
  :linenos:

    // Old initialization
    DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id);

    // Replace with new initialization
    DFTRACER_CPP_INIT(log_file, data_dirs, process_id);

This will configure the DFTracer environment, setting up the log file, data directories, and process ID exactly like the DLIO Profiler did.
To migrate these configurations from DLIO Profile to DFTracer please replace your old enviromental variable configurations as shown bellow.

.. code-block:: bash
   :linenos:
    # Old environment variable configurations for DLIO Profiler
    DLIO_LOG_FILE=~/dlio_log
    DLIO_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset
    export DLIO_INIT=PRELOAD
    export DLIO_ENABLE=1


.. code-block:: bash
   :linenos:
    # Updated environment variable configurations for DFTracer
    DFTRACER_LOG_FILE=~/log_file  # Changes the log file path variable name
    DFTRACER_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset  # Consistent data directory path
    export DFTRACER_INIT=PRELOAD  # Standardizing to PRELOAD mode
    export DFTRACER_ENABLE=1  # Enabling the profiler


Finalizing
----------------------

The finalization process ensures that all tracing data are correctly finalized and saved. Replace the DLIO Profiler finalization call with the DFTracer finalization.

.. code-block:: cpp
  :linenos:

    // Old finalization
    DLIO_PROFILER_CPP_FINI();

    // Replace with new finalization
    DFTRACER_CPP_FINI();

This function call is crucial for ensuring that your profiling data is not corrupted and is properly written to the log file.

Function and Region Profiling
-----------------------------------

For function and code block profiling, replace the old DLIO Profiler functions with their DFTracer counterparts.

.. code-block:: cpp
  :linenos:

    // Old function and region profiling
    DLIO_PROFILER_CPP_FUNCTION();
    DLIO_PROFILER_CPP_REGION_<START/END>(CUSTOM);

    // Replace with new function and region profiling
    DFTRACER_CPP_FUNCTION();
    DFTRACER_CPP_REGION_<START/END>(CUSTOM);


------------------------------------------------
C API Changes
------------------------------------------------

This section guides you through the necessary changes to migrate your application-level tracing for C projects from DLIO Profiler to DFTracer. The transition requires updating API names and includes directives to use the DFTracer's new API.
Please see `examples.rst` for more information on how to use DFTracer APIs.

Updating Includes
---------------------

To transition your C projects to DFTracer, begin by updating the include directive to point to the new DFTracer API.

.. code-block:: c
   :linenos:

    // Old include
    #include <dlio_profiler/dlio_profiler.h>

    // Replace with new include
    #include <dftracer/dftracer.h>

Initializing
------------------------

For C applications, DFTracer initialization replaces the older DLIO Profiler calls.

.. code-block:: c
   :linenos:

    // Old initialization
    DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id);

    // Replace with new initialization
    DFTRACER_C_INIT(log_file, data_dirs, process_id);

This command configures DFTracer with the necessary parameters for logging and directory monitoring, similarly to how DLIO Profiler was configured.
To migrate these configurations from DLIO Profile to DFTracer please replace your old enviromental variable configurations as shown bellow.

.. code-block:: bash
   :linenos:

    # Old environment variable configurations for DLIO Profiler
    DLIO_LOG_FILE=~/dlio_log
    DLIO_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset
    export DLIO_INIT=PRELOAD
    export DLIO_ENABLE=1


.. code-block:: bash
   :linenos:

    # Updated environment variable configurations for DFTracer
    DFTRACER_LOG_FILE=~/log_file  # Changes the log file path variable name
    DFTRACER_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset  # Consistent data directory path
    export DFTRACER_INIT=PRELOAD  # Standardizing to PRELOAD mode
    export DFTRACER_ENABLE=1  # Enabling the profiler


Finalizing
----------------------

Finalize the DFTracer setup to ensure all tracing data are correctly captured and saved.

.. code-block:: c
   :linenos:

    // Old finalization
    DLIO_PROFILER_C_FINI();

    // Replace with new finalization
    DFTRACER_C_FINI();


Function and Region Profiling
-----------------------------------

Transition function and region profiling in your C code to use DFTracer's updated API methods.

.. code-block:: c
   :linenos:

    // Old function and region profiling
    DLIO_PROFILER_C_FUNCTION_START();
    DLIO_PROFILER_C_FUNCTION_END();

    // Replace with new function and region profiling
    DFTRACER_C_FUNCTION_START();
    DFTRACER_C_FUNCTION_END();


------------------------------------------------
Python API changes
------------------------------------------------

------------------------------------------------
Application building migration
------------------------------------------------
To migrate your Makefile projects from using DLIO Profiler to DFTracer, you will need to update your compilation flags, specifically `CFLAGS` or `CXXFLAGS`, and `LDFLAGS`. 
Replace the DLIO Profiler flags with DFTracer flags as shown below:

.. code-block:: make
   :linenos:
   :caption: Modifying Makefile to use DFTracer

   # DLIO Profiler Flags (old)
   DLIO_CFLAGS = -I/usr/workspace/iopp/kogiou1/venvs/pegasus-env/lib/python3.9/site-packages/dlio_profiler/include
   DLIO_LDFLAGS = -L/usr/workspace/iopp/kogiou1/venvs/pegasus-env/lib/python3.9/site-packages/dlio_profiler/lib64 -ldlio_profiler
   CFLAGS += $(DLIO_CFLAGS)
   LIBS += $(DLIO_LDFLAGS)

   # Replace with DFTracer Flags (new)
   # Add DFTracer include and library paths
   DFTRACER_CFLAGS = -I/path/to/dftracer/include
   DFTRACER_LDFLAGS = -L/path/to/dftracer/lib64 -ldftracer

   # Append to existing CFLAGS and LDFLAGS
   CFLAGS += $(DFTRACER_CFLAGS)
   LDFLAGS += $(DFTRACER_LDFLAGS)

------------------------------------------------
C++ API Changes
------------------------------------------------

This section guides you through the necessary changes to migrate your application-level tracing for C++ projects from DLIO Profiler to DFTracer. The transition requires updating API names and includes directives to use the DFTracer's new API.
Please see `examples.rst` for more information on how to use DFTracer APIs.

Updating Includes
---------------------

Replace the old DLIO Profiler include header with the new DFTracer header. This change points your application to the new tracing API.

.. code-block:: cpp
  :linenos:

    // Old include
    #include <dlio_profiler/dlio_profiler.h>

    // Replace with new include
    #include <dftracer/dftracer.h>

Initializing
------------------------

Initialization now uses the DFTracer API, which can seamlessly integrate into your existing codebase where DLIO Profiler was previously initialized. 

.. code-block:: cpp
  :linenos:

    // Old initialization
    DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id);

    // Replace with new initialization
    DFTRACER_CPP_INIT(log_file, data_dirs, process_id);

This will configure the DFTracer environment, setting up the log file, data directories, and process ID exactly like the DLIO Profiler did.
To migrate these configurations from DLIO Profile to DFTracer please replace your old enviromental variable configurations as shown bellow.


.. code-block:: bash
   :linenos:
   
    # Old environment variable configurations for DLIO Profiler
    DLIO_LOG_FILE=~/dlio_log
    DLIO_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset
    export DLIO_INIT=PRELOAD
    export DLIO_ENABLE=1


.. code-block:: bash
   :linenos:
   
    # Updated environment variable configurations for DFTracer
    DFTRACER_LOG_FILE=~/log_file  # Changes the log file path variable name
    DFTRACER_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset  # Consistent data directory path
    export DFTRACER_INIT=PRELOAD  # Standardizing to PRELOAD mode
    export DFTRACER_ENABLE=1  # Enabling the profiler


Finalizing
----------------------

The finalization process ensures that all tracing data are correctly finalized and saved. Replace the DLIO Profiler finalization call with the DFTracer finalization.

.. code-block:: cpp
  :linenos:

    // Old finalization
    DLIO_PROFILER_CPP_FINI();

    // Replace with new finalization
    DFTRACER_CPP_FINI();

This function call is crucial for ensuring that your profiling data is not corrupted and is properly written to the log file.

Function and Region Profiling
-----------------------------------

For function and code block profiling, replace the old DLIO Profiler functions with their DFTracer counterparts.

.. code-block:: cpp
  :linenos:

    // Old function and region profiling
    DLIO_PROFILER_CPP_FUNCTION();
    DLIO_PROFILER_CPP_REGION_<START/END>(CUSTOM);

    // Replace with new function and region profiling
    DFTRACER_CPP_FUNCTION();
    DFTRACER_CPP_REGION_<START/END>(CUSTOM);


------------------------------------------------
C API Changes
------------------------------------------------

This section guides you through the necessary changes to migrate your application-level tracing for C projects from DLIO Profiler to DFTracer. The transition requires updating API names and includes directives to use the DFTracer's new API.
Please see `examples.rst` for more information on how to use DFTracer APIs.

Updating Includes
---------------------

To transition your C projects to DFTracer, begin by updating the include directive to point to the new DFTracer API.

.. code-block:: c
   :linenos:

    // Old include
    #include <dlio_profiler/dlio_profiler.h>

    // Replace with new include
    #include <dftracer/dftracer.h>

Initializing
------------------------

For C applications, DFTracer initialization replaces the older DLIO Profiler calls.

.. code-block:: c
   :linenos:

    // Old initialization
    DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id);

    // Replace with new initialization
    DFTRACER_C_INIT(log_file, data_dirs, process_id);

This command configures DFTracer with the necessary parameters for logging and directory monitoring, similarly to how DLIO Profiler was configured.
To migrate these configurations from DLIO Profile to DFTracer please replace your old enviromental variable configurations as shown bellow.

.. code-block:: bash
   :linenos:

    # Old environment variable configurations for DLIO Profiler
    DLIO_LOG_FILE=~/dlio_log
    DLIO_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset
    export DLIO_INIT=PRELOAD
    export DLIO_ENABLE=1


.. code-block:: bash
   :linenos:

    # Updated environment variable configurations for DFTracer
    DFTRACER_LOG_FILE=~/log_file  # Changes the log file path variable name
    DFTRACER_DATA_DIR=/dev/shm/:/p/gpfs1/$USER/dataset  # Consistent data directory path
    export DFTRACER_INIT=PRELOAD  # Standardizing to PRELOAD mode
    export DFTRACER_ENABLE=1  # Enabling the profiler


Finalizing
----------------------

Finalize the DFTracer setup to ensure all tracing data are correctly captured and saved.

.. code-block:: c
   :linenos:

    // Old finalization
    DLIO_PROFILER_C_FINI();

    // Replace with new finalization
    DFTRACER_C_FINI();


Function and Region Profiling
-----------------------------------

Transition function and region profiling in your C code to use DFTracer's updated API methods.

.. code-block:: c
   :linenos:

    // Old function and region profiling
    DLIO_PROFILER_C_FUNCTION_START();
    DLIO_PROFILER_C_FUNCTION_END();

    // Replace with new function and region profiling
    DFTRACER_C_FUNCTION_START();
    DFTRACER_C_FUNCTION_END();


------------------------------------------------
Python API changes
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


