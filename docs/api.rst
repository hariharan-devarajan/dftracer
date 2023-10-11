======================
DLIO Profiler APIs
======================

In this document, we detail how to use DLIO Profiler APIs for languages.
Please refer to `chrome tracing document`_ for definition on ``cat``, ``name``, and ``event``.

======================
Modes of DLIO Profiler
======================

The DLIO Profiler can be used in three main modes.

1. APP MODE: In this mode, the application can use function/region level API's to profile application codes.
2. PRELOAD MODE: In this mode, the application can transperantly load up DLIO Profiler to profile POSIX or STDIO calls.
3. HYBRID MODE: In this mode, application can use both APP and PRELOAD Mode to perform I/O from all dynamically spawned processes and function profiling from application.

===============================
Configurations of DLIO Profiler
===============================

.. table:: section - main configuration settings
   :widths: auto

   ================================ ======  ===========================================================================
   Environment Varibale             Type    Description
   ================================ ======  ===========================================================================
   DLIO_PROFILER_ENABLE             INT     Enable or Disable DLIO Profiler (default 0).
   DLIO_PROFILER_INIT               STRING  DLIO Profiler Mode FUNCTION/PRELOAD (default FUNCTION).
                                            For Hybrid use PRELOAD mode.
   DLIO_PROFILER_LOG_FILE           STRING  PATH To log file. In this case process id and app name is appended to file.
   DLIO_PROFILER_DATA_DIR           STRING  Colon separated paths that will be traced for I/O accesses by profiler.
   DLIO_PROFILER_INC_METADATA       INT     Include or exclude metadata (default 0)
   DLIO_PROFILER_SET_CORE_AFFINITY  INT     Include or exclude core affinity (default 0).
                                            DLIO_PROFILER_INC_METADATA needs to be enabled.
   DLIO_PROFILER_GOTCHA_PRIORITY    INT     PRIORITY of DLIO Profiler in GOTCHA (default: 1).
   DLIO_PROFILER_LOG_LEVEL          STRING  Logging level within DLIO Profiler ERROR/WARN/INFO/DEBUG (default ERROR).
   DLIO_PROFILER_DISABLE_IO         STRING  Disable automatic binding of all I/O calls.
   DLIO_PROFILER_DISABLE_POSIX      STRING  Disable automatic binding of POSIX I/O calls.
   DLIO_PROFILER_DISABLE_STDIO      STRING  Disable automatic binding of STDIO I/O calls.
   ================================ ======  ===========================================================================

======================
DLIO Profiler C++ APIs
======================

This section describes how to use DLIO Profiler for profling C++ application using C++ APIs.

-----

----------------------------------------
Include the DLIO Profiler Header for C++
----------------------------------------

In C or C++ applications, include ``dlio_profiler/dlio_profiler.h``.

.. code-block:: c

    #include <dlio_profiler/dlio_profiler.h>


-------------------------------
Initialization of DLIO Profiler
-------------------------------

To initalize DLIO Profiler for application, applications needs to use ``DLIO_PROFILER_CPP_INIT``.
We need to pass ``log_file``, ``data directories``, and ``process id`` to be used by the profiler.
``data_dirs`` is a parameter which is a Colon ``:`` separated PATH of directories to be traced.
If users pass nullptr to log_file and data_dirs, the environment variables for these will be used.
Additionally, if users pass nullptr to process_id, then getpid() function would be used to automatically fetch process id.

.. code-block:: c

    DLIO_PROFILER_CPP_INIT(log_file, data_dirs, process_id);

-------------------------------
Finalization of DLIO Profiler
-------------------------------

Finalization call to clean DLIO Profiler entries (Optional). If users do not call this, they have to manually add ``[`` at the start of the log file.

.. code-block:: c

    DLIO_PROFILER_CPP_FINI();


------------------
Function Profiling
------------------

To profile a function, add the wrapper ``DLIO_PROFILER_CPP_FUNCTION`` at the start of the function

.. code-block:: c
    void foo() {
      DLIO_PROFILER_CPP_FUNCTION();
      sleep(1);
    } // DLIO_PROFILER_CPP_FUNCTION ends here.

--------------------------------------
Region Level Profiling for Code blocks
--------------------------------------

To profile a block of code which has a scope using ``{ .... }``, we can use ``DLIO_PROFILER_CPP_REGION(<NAME>)``.
The name of the region should unique within the scope of the function/code block.

.. code-block:: c

    void foo() {
      DLIO_PROFILER_CPP_FUNCTION();
      sleep(1);
      {
        DLIO_PROFILER_CPP_REGION(CUSTOM);
        sleep(1);

      } // DLIO_PROFILER_CPP_REGION ends here implicitly
    } // DLIO_PROFILER_CPP_FUNCTION ends here.

----------------------------------------
Region Level Profiling for lines of code
----------------------------------------

To profile a specific set of lines within your code, use the ``DLIO_PROFILER_CPP_REGION_<START/END>`` APIs.
The ``START`` and ``END`` calls should be in the same scope of the function.

.. code-block:: c

    void foo() {
      DLIO_PROFILER_CPP_FUNCTION();
      sleep(1);
      {
        DLIO_PROFILER_CPP_REGION(CUSTOM);
        sleep(1);
        DLIO_PROFILER_CPP_REGION_START(CUSTOM_BLOCK);
        sleep(1);
        DLIO_PROFILER_CPP_REGION_END(CUSTOM_BLOCK); // CUSTOM_BLOCK started by DLIO_PROFILER_CPP_REGION_START ends
      } // DLIO_PROFILER_CPP_REGION ends here implicitly
    } // DLIO_PROFILER_CPP_FUNCTION ends here.


======================
DLIO Profiler C APIs
======================

This section describes how to use DLIO Profiler for profling C application using C APIs.

-----

--------------------------------------
Include the DLIO Profiler Header for C
--------------------------------------

In C application, include ``dlio_profiler/dlio_profiler.h``.

.. code-block:: c

    #include <dlio_profiler/dlio_profiler.h>

-------------------------------
Initialization of DLIO Profiler
-------------------------------

To initalize DLIO Profiler for application, applications needs to use ``DLIO_PROFILER_C_INIT``.
We need to pass ``log_file``, ``data directories``, and ``process id`` to be used by the profiler.
``data_dirs`` is a parameter which is a Colon ``:`` separated PATH of directories to be traced.
If users pass NULL to log_file and data_dirs, the environment variables for these will be used.
Additionally, if users pass NULL to process_id, then getpid() function would be used to automatically fetch process id.

.. code-block:: c

    DLIO_PROFILER_C_INIT(log_file, data_dirs, process_id);

-------------------------------
Finalization of DLIO Profiler
-------------------------------

Finalization call to clean DLIO Profiler entries (Optional). If users do not call this, they have to manually add ``[`` at the start of the log file.

.. code-block:: c

    DLIO_PROFILER_C_FINI();

------------------
Function Profiling
------------------

To profile a function, add the wrapper ``DLIO_PROFILER_C_FUNCTION_START`` at the start of the function and
``DLIO_PROFILER_C_FUNCTION_END`` at the end of the function.

.. code-block:: c

    void foo() {
      DLIO_PROFILER_C_FUNCTION_START();
      sleep(1);
      if (<CONDITION>) {
        DLIO_PROFILER_C_FUNCTION_END();
        return; // Define DLIO_PROFILER_C_FUNCTION_END on every branch
      }
      DLIO_PROFILER_C_FUNCTION_END(); // Define DLIO_PROFILER_C_FUNCTION_END on every branch
    }

.. attention::

    For capturing all code branches, every return statement should have a corresponding ``DLIO_PROFILER_C_FUNCTION_END`` block within the function.

----------------------------------------
Region Level Profiling for lines of code
----------------------------------------

To profile a specific set of lines within your code, use the ``DLIO_PROFILER_C_REGION_<START/END>`` APIs.
The ``START`` and ``END`` calls should be in the same scope of the function.
The name passed to the function should be unique in every scope.

.. code-block:: c

    void foo() {
      DLIO_PROFILER_C_FUNCTION_START();
      sleep(1);
      DLIO_PROFILER_C_REGION_START(CUSTOM);
      sleep(1);
      DLIO_PROFILER_C_REGION_END(CUSTOM); // END region CUSTOM.
      DLIO_PROFILER_C_FUNCTION_END(); // END FUNCTION foo.
    }

======================
DLIO Profiler Python APIs
======================

This section describes how to use DLIO Profiler for profling python applications.

-----

--------------------------------
Include the DLIO Profiler module
--------------------------------

In C application, include ``dlio_profiler/dlio_profiler.h``.

.. code-block:: python

    from dlio_profiler.logger import dlio_logger

-------------------------------
Initialization of DLIO Profiler
-------------------------------

To initalize DLIO Profiler for application, applications needs to use ``dlio_logger.initialize_log``.
We need to pass ``log_file``, ``data directories``, and ``process id`` to be used by the profiler.
``data_dir`` is a parameter which is a Colon ``:`` separated PATH of directories to be traced.
If users pass None to log_file and data_dirs, the environment variables for these will be used.
Additionally, if users pass -1 to process_id, then getpid() function would be used to automatically fetch process id.

.. code-block:: python

    dlp_logger = dlio_logger.initialize_log(logfile, data_dir, process_id)

-------------------------------
Finalization of DLIO Profiler
-------------------------------

Finalization call to clean DLIO Profiler entries (Optional). If users do not call this, they have to manually add ``[`` at the start of the log file.

.. code-block:: python

    dlp_logger.finalize()


----------------------------------
Function decorator style profiling
----------------------------------

With python applications, developers can use decorator provided within dlio_profiler to tag functions that need to be profiled.
To use the function decorators, they can be initialized in place or globally to reuse within many functions.
The ``fn_interceptor`` is the decorator for the application.
It takes two arguments: 1) ``cat`` represents the category for the event and 2) an optional ``name`` represents the name of the event.
In general, the name of the event can be automatically loaded by the function during decoration as well.

.. code-block:: python

    from dlio_profiler.logger import fn_interceptor
    dlio_log = fn_interceptor("COMPUTE")

    @dlio_log.log
    def log_events(index):
        sleep(1)

For logging ``__init__`` function within a class, applications can use ``log_init`` function.

.. code-block:: python

    from dlio_profiler.logger import fn_interceptor
    dlio_log = fn_interceptor("COMPUTE")

    class Test:
        @dlio_log.log_init
        def __init__(self):
            sleep(1)

        @dlio_log.log
        def log_events(self, index):
            sleep(1)

-------------------------
Iteration/Loop Profiling
-------------------------

For logging every block within a loop, we have an ``fn_interceptor.iter`` which takes a generator function and wraps around the element yield block.

.. code-block:: python

    from dlio_profiler.logger import fn_interceptor
    dlio_log = fn_interceptor("COMPUTE")

    for batch in dlio_log.iter(loader.next()):
        sleep(1)

-----------------------
Context style Profiling
-----------------------

We can also profile a block of code using Python's context managers using ``fn_interceptor``.

.. code-block:: python

    from dlio_profiler.logger import fn_interceptor
    with fn_interceptor(cat="block", name="step") as dlp:
        sleep(1)
        dlp.update(step=1)

-----------------------
Clustom Profiling
-----------------------

Lastly, users can use specific logger entries to log events within their application.
In general this should be only used when other cases cannot be applied.

.. code-block:: python

    from dlio_profiler.logger import dlio_logger
    dlp_logger = dlio_logger.initialize_log(logfile, data_dir, process_id)
    start = dlp_logger.get_time()
    sleep(1)
    end = dlp_logger.get_time()
    dlp_logger.log_event(name="test", cat="cat2", start, end - start, int_args=args)

.. _`chrome tracing document`: https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview#heading=h.yr4qxyxotyw
.. _symbol: https://refspecs.linuxfoundation.org/LSB_3.0.0/LSB-PDA/LSB-PDA.junk/symversion.html
