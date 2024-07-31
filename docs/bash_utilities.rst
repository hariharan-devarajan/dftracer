========================
Bash Utility scripts 
========================

This section describes the bash utilities that are compatible with DFTracer logs

----------

------------------
Handling .pfw files
------------------

The DFTracer format with extension .pfw is uncompressed file which can be viewed using the following utilities.

1. `vim` : Edit .pfw files
2. `cat`, `head`, or `tail`: view portion of the pfw files


----------------------
Handling .pfw.gz files
----------------------

The DFtracer compressed format with .pfw extension can be first decompressed using gzip and then piped to the above .pfw utilities.

.. code-block:: bash

    gzip -c -d `echo *.gz` | head

--------------------
Extracting JSON data
--------------------

Once the uncompressed data is parsed. The JSON utility `jq` can be used to parse args.

In each case we have to remove the first `[` which has been added to support perfetto ui.

For uncompressed files

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '.'


For compressed files

.. code-block:: bash

    gzip -c -d `echo *.gz` | grep -i "[^#[]" | jq -c '.'

We can extract specific fields from these JSON lines as follows

1. `jq -c '.name'`: extracts all the names of events
2. `jq -c '.cat'`: extracts all the category of events
3. `jq -c '.args.hostname'`: extracts the fields from extra args like hostname in this case.

Useful querying using jq
************************

Extract unique functions with their counts from traces.

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '.name' | sort | uniq -c 

Extract unique categories with their counts from traces.

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '.cat' | sort | uniq -c 

Extract unique process id and thread id combination with their counts from traces.

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '"\(.pid) \(.tid)"' | sort | uniq -c 

Extract min timestamp

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '.ts | tonumber' | sort -n | tail -1

Extract max timestamp

.. code-block:: bash

    cat *.pfw | grep -i "[^#[]" | jq -c '.ts | tonumber' | sort -n | tail -n 1


For more commands on `jq` refer to  `JQ Manual
<https://jqlang.github.io/jq/manual/>`_.

