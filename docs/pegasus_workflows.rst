===================================
Pegasus Montage with DFTracer
===================================

Instructions for tracing Pegasus Montage with DFTracer on LC Corona. 
These instructions can be used for any Workflow but you'll need to change the version of the tar files depending on 
the architecture of your machine and the workflow you are interested in. 
For more information, visit the `workflows repository <https://github.com/OlgaKogiou/workflows>`_.

Step 1: Install Condor
----------------------

1.1 Get the zip:
.. code-block:: bash

    wget https://research.cs.wisc.edu/htcondor/tarball/10.x/current/condor-x86_64_CentOS8-stripped.tar.gz

1.2 Untar to your condor folder:
.. code-block:: bash

    tar -x -f condor*.tar.gz
    mkdir condor
    cd condor-*stripped
    mv * ../condor
    cd ..
    rm -rf condor-*stripped
    rm condor-stripped.tar.gz

1.3 Configure:
.. code-block:: bash

    cd condor
    ./bin/make-personal-from-tarball

Step 2: Install Pegasus
-----------------------

2.1 Get the zip from Tarballs:
.. code-block:: bash

    wget https://download.pegasus.isi.edu/pegasus/5.0.7/pegasus-binary-5.0.7-x86_64_rhel_7.tar.gz
    wget https://download.pegasus.isi.edu/pegasus/5.0.7/pegasus-worker-5.0.7-x86_64_rhel_7.tar.gz

2.2 Untar to your Pegasus folder (both for pegasus and pegasus-worker):
.. code-block:: bash

    tar zxf pegasus-*.tar.gz
    rm pegasus-*.tar.gz

Step 3: Install and compile Montage
-----------------------------------

3.1 Get the code:
.. code-block:: bash

    git clone https://github.com/Caltech-IPAC/Montage.git

3.2 Compile:
.. code-block:: bash

    cd Montage
    make

Note: Make sure there are no errors. By cloning the GitHub repo, you get the most recent version, likely with no compiler errors. cd Montage/bin and make sure it is not empty.

3.3 Save in Paths:
.. code-block:: bash

    export PATH=/path/to/Montage/bin:$PATH

Step 4: Get the montage-pegasus-v3
----------------------------------

4.1 Create and activate Virtual Environment:
.. code-block:: bash

    python3 -m venv /path/to/pegasus-env
    source /path/to/pegasus-env/bin/activate

4.2 Install dependencies:
.. code-block:: bash

    pip install astropy
    pip install pegasus-wms
    pip install git+https://github.com/hariharan-devarajan/dftracer.git

4.3 Get the code:
.. code-block:: bash

    git clone https://github.com/pegasus-isi/montage-workflow-v3.git

Step 5: Compile the pegasus-mpi-cluster from source
---------------------------------------------------

5.1 Get the code:
.. code-block:: bash

    git clone https://github.com/pegasus-isi/pegasus.git

5.2 Make sure you’re in the virtual environment for Pegasus:
.. code-block:: bash

    source /path/to/pegasus-env/bin/activate

5.3 Make sure you have the prerequisites:

    - Git
    - Java 8 or higher
    - Python 3.5 or higher
    - R
    - Ant
    - gcc
    - g++
    - make
    - tox 3.14.5 or higher
    - mysql (optional, required to access MySQL databases)
    - postgresql (optional, required to access PostgreSQL databases)
    - Python pyyaml
    - Python GitPython

5.4 Compile:
.. code-block:: bash

    cd pegasus
    ant compile-pegasus-mpi-cluster

5.5 Copy it to your Pegasus folder:
.. code-block:: bash

    cd packages/pegasus-mpi-cluster/
    cp pegasus-mpi-cluster/ /path/to/pegasus-5.0.7/bin

Note: If errors occur while compiling, make sure that MVAPICH is loaded:
.. code-block:: bash

    module load mvapich2-tce/2.3.7
    echo $LD_LIBRARY_PATH

Step 6: Create a single “install” directory for all Pegasus software
--------------------------------------------------------------------

This will help in resolving errors like “cannot find .. in your path”.

6.1 Move into the Pegasus directory (the one you compiled from source) and make a directory called install:
.. code-block:: bash

    cd pegasus
    mkdir install

6.2 Copy all components from pegasus-5.0.7 and condor into the pegasus/install folder:
.. code-block:: bash

    cd ../condor
    cp * ../pegasus/install
    cp -r * ../pegasus/install
    cd ../pegasus-5.0.7
    cp * ../pegasus/install
    cp -r * ../pegasus/install

Note: If you encounter errors about overwriting /bin or /lib folders, you have to do it manually by cd into those folders and copying everything to /pegasus/install/bin or /pegasus/install/lib. Make sure all components are there, otherwise Pegasus and Condor cannot run.

Step 7: Run workflows with Pegasus
----------------------------------

7.1 Make sure you are in the virtual environment still. If not, source it again by repeating 5.2.

7.2 Save to PATH:
.. code-block:: bash

    export PATH=/path/to/pegasus/install/bin:$PATH
    export PATH=/path/to/pegasus/install/sbin:$PATH
    export LD_LIBRARY_PATH=/path/to/pegasus/install//lib:$LD_LIBRARY_PATH
    source ~/.bashrc

7.3 Run Condor:
.. code-block:: bash

    chmod 777 /path/to/pegasus/install/condor.sh
    . /path/to/pegasus/install/condor.sh
    condor_master
    condor_status  # it should show the activity
    condor_q  # it should show the jobs running

Note: If errors occur, echo the LD_LIBRARY_PATH and the PATH and make sure /pegasus/install is there.

To check if condor_shedd and all other condor processes are running:
.. code-block:: bash

    ps aux | grep condor

If Condor throws errors while trying to connect to another node:

1. Exit the flux allocation:
.. code-block:: bash

    exit

2. Check your processes:
.. code-block:: bash

    ps -u USER

3. Kill all your processes (or those related to Condor if any):
.. code-block:: bash

    killall -u USER

4. Repeat steps 6.3, 6.4, 5.2, 6.5

5. If the problem persists:
.. code-block:: bash

    condor_restart

7.4 Test Pegasus:
.. code-block:: bash

    pegasus-version  # should show 5.0.7

Note: If error "Cannot find file with permissions" occurs, touch that file and make sure it has those permissions.

7.5 Configure the Condor/SLURM interface:
.. code-block:: bash

    pegasus-configure-glite

Note: If error "Cannot find file with permissions" occurs, touch that file and make sure it has those permissions.

7.6 Configure the DFTracer flags:

.. code-block:: bash

    export DFTRACER_INSTALLED=/path/to/pegasus-env/lib/python3.9/site-packages/dftracer/
    export LD_LIBRARY_PATH=$DFTRACER_INSTALLED/lib:$DFTRACER_INSTALLED/lib64:$LD_LIBRARY_PATH
    export DFTRACER_LOG_FILE=/path/to/traces/trace
    # export DFTRACER_DATA_DIR=all (optional)
    export DFTRACER_ENABLE=1
    export DFTRACER_INC_METADATA=1
    # export DFTRACER_INIT=PRELOAD (optional)
    export DFTRACER_BIND_SIGNALS=0
    # export DFTRACER_LOG_LEVEL=ERROR (optional)
    export DFTRACER_TRACE_COMPRESSION=1 

    # dftracer=$DFTRACER_INSTALLED/lib64/libdftracer_preload.so (optional)

You would only need to use the preload version of DFTracer if you have not annotated the application code you are running.
For more information on the flags and their functionalities please turn to :docs:`examples`.
