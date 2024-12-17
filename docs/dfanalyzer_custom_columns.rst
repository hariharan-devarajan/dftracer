===========================================
Custom Columns Derived from DFTracer
===========================================

This section describes how to derive custom columns for a DFAnalyzer data framework from DFTracer events using a custom function and load specific fields.

Example: Custom Workflow Columns
================================

We can define custom columns in `DFAnalyzer` by specifying a function that extracts the desired fields from the `json_object` and then loading those fields with their corresponding types.

Below is an example of how to define a custom function to derive columns from a Pegasus Montage workflow trace:

.. code-block:: python

    def wf_cols_function(json_object, current_dict, time_approximate, condition_fn, load_data):
        d = {}
        if "args" in json_object:
            if "size" in json_object["args"]:
                d["size"] = int(json_object["args"]["size"])
            if "ret" in json_object["args"]:
                d["ret"] = int(json_object["args"]["ret"])
        return d

    load_cols_wf = {'size': "int64[pyarrow]", 'ret': "int64[pyarrow]"}

Next, use this function in `DFAnalyzer` to load traces with the custom columns (here is an example of loading Montage traces):

.. code-block:: python

    from dfanalyzer import DFAnalyzer

    analyzer = DFAnalyzer(
        "/path/to/montage-*-preload.pfw.gz", 
        load_fn=wf_cols_function, 
        load_cols=load_cols_wf
    )

Here, the custom columns `size`, `ret`, and `cmd` are loaded into the `DFAnalyzer` using the `wf_cols_function`.

You can modify the function and column types to match the fields relevant to your workload.
