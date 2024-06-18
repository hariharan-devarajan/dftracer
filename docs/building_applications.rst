=========================================================
Building applications with DLIO Profiler Code Annotations
=========================================================

This section provides guidelines on how to build applications with DLIO Profiler Code Annotations. 

Building for Projects with Makefiles
------------------------------------

To integrate DLIO Profiler into your Makefile projects, you need to modify the `CFLAGS` or `CXXFLAGS` and `LDFLAGS` in your Makefile to include the DLIO Profiler's headers and libraries. Below are the steps:

1. **Set the DLIO Profiler include and library paths:**

   Ensure you know the paths where DLIO Profiler's header files and libraries are located. Typically, they might be in directories like `/path/to/dlio-profiler/include` and `/path/to/dlio-profiler/lib64`. If you have built DLIO Profiler with pip then these paths may look like `/path/to/lib/pythonX/site-packages/dlio_profiler/include` and `/path/to/lib/pythonX/site-packages/dlio_profiler/lib64`.

2. **Modify your Makefile:**

   Open your Makefile and locate the sections where `CFLAGS` or `CXXFLAGS` and `LDFLAGS` are defined. Add the include path to `CFLAGS` or `CXXFLAGS` and the library path to `LDFLAGS`.

   Here is an example snippet of how to modify a Makefile to build a C program:

   .. code-block:: makefile

      # Existing CFLAGS and LDFLAGS
      CFLAGS = -g -O2 -std=c99
      LDFLAGS = -lm

      # Add DLIO Profiler include and library paths
      DLIO_CFLAGS = -I/path/to/dlio-profiler/include
      DLIO_LDFLAGS = -L/path/to/dlio-profiler/lib64 -ldlio_profiler

      # Append to existing CFLAGS and LDFLAGS
      CFLAGS += $(DLIO_CFLAGS)
      LDFLAGS += $(DLIO_LDFLAGS)

3. **Use the DLIO Profiler macros in your source code:**

   Ensure that you include the DLIO Profiler header in your source files and use the profiling macros as needed.

   Example for a C program:

   .. code-block:: c

      #include <dlio_profiler/dlio_profiler.h>

      void some_function_with_annotations() {
          DLIO_PROFILER_C_FUNCTION_START();
          sleep(1);
          DLIO_PROFILER_C_FUNCTION_END();
          return;          
      }

4. **Build your project:**

   Run `make` as usual to build your project with DLIO Profiler annotations integrated.

   .. code-block:: sh

      make


