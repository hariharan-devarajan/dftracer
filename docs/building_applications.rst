=========================================================
Building applications with DFTracer Code Annotations
=========================================================

This section provides guidelines on how to build applications with DFTracer Code Annotations. 

Building for Projects with Makefiles
------------------------------------

To integrate DFTracer into your Makefile projects, you need to modify the `CFLAGS` or `CXXFLAGS` and `LDFLAGS` in your Makefile to include the DFTracer's headers and libraries. Below are the steps:

1. **Set the DFTracer include and library paths:**

   Ensure you know the paths where DFTracer's header files and libraries are located. Typically, they might be in directories like `/path/to/dftracer/include` and `/path/to/dftracer/lib64`. If you have built DFTracer with pip then these paths may look like `/path/to/lib/pythonX/site-packages/dftracer/include` and `/path/to/lib/pythonX/site-packages/dftracer/lib64`.

2. **Modify your Makefile:**

   Open your Makefile and locate the sections where `CFLAGS` or `CXXFLAGS` and `LDFLAGS` are defined. Add the include path to `CFLAGS` or `CXXFLAGS` and the library path to `LDFLAGS`.

   Here is an example snippet of how to modify a Makefile to build a C program:

   .. code-block:: makefile

      # Existing CFLAGS and LDFLAGS
      CFLAGS = -g -O2 -std=c99
      LDFLAGS = -lm

      # Add DFTracer include and library paths
      DFTRACER_CFLAGS = -I/path/to/dftracer/include
      DFTRACER_LDFLAGS = -L/path/to/dftracer/lib64 -ldftracer

      # Append to existing CFLAGS and LDFLAGS
      CFLAGS += $(DFTRACER_CFLAGS)
      LDFLAGS += $(DFTRACER_LDFLAGS)

3. **Use the DFTracer macros in your source code:**

   Ensure that you include the DFTracer header in your source files and use the profiling macros as needed.

   Example for a C program:

   .. code-block:: c

      #include <dftracer/dftracer.h>

      void some_function_with_annotations() {
          DFTRACER_C_FUNCTION_START();
          sleep(1);
          DFTRACER_C_FUNCTION_END();
          return;          
      }

4. **Build your project:**

   Run `make` as usual to build your project with DFTracer annotations integrated.

   .. code-block:: sh

      make


