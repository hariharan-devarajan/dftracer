===================
Build DLIO Profiler
===================

This section describes how to build DLIO Profiler.

There are three build options:

- build DLIO Profiler with pip (recommended),
- build DLIO Profiler with Spack, and
- build DLIO Profiler with cmake

----------

----------------------------
Build DLIO Profiler with pip
----------------------------

Users can easily install DLIO profiler using pip. This is the way most python packages are installed.
This method would work for both native python environments and conda environments.

From source
************

.. code-block:: Bash

    git clone git@github.com:hariharan-devarajan/dlio-profiler.git
    cd dlio-profiler
    # You can skip this for installing the dev branch.
    # for latest stable version use master branch.
    git checkout tags/<Release> -b <Release>
    pip install .

From Github
************

.. code-block:: Bash

  DLP_VERSION=dev
  pip install git+https://github.com/hariharan-devarajan/dlio-profiler.git@${DLP_VERSION}

-----------------------------------------
Build DLIO Profiler with Spack
-----------------------------------------


One may install DLIO Profiler with Spack_.
If you already have Spack, make sure you have the latest release.
If you use a clone of the Spack develop branch, be sure to pull the latest changes.

.. _build-label:

Install Spack
*************
.. code-block:: Bash

    $ git clone https://github.com/spack/spack
    $ # create a packages.yaml specific to your machine
    $ . spack/share/spack/setup-env.sh

Use `Spack's shell support`_ to add Spack to your ``PATH`` and enable use of the
``spack`` command.

Build and Install DLIO Profiler
*******************************

.. code-block:: Bash

    $ spack install py-dlio-profiler-py
    $ spack load py-dlio-profiler-py

If the most recent changes on the development branch ('dev') of DLIO Profiler are
desired, then do ``spack install py-dlio-profiler-py@develop``.

.. attention::

    The initial install could take a while as Spack will install build
    dependencies (autoconf, automake, m4, libtool, and pkg-config) as well as
    any dependencies of dependencies (cmake, perl, etc.) if you don't already
    have these dependencies installed through Spack or haven't told Spack where
    they are locally installed on your system (i.e., through a custom
    packages.yaml_).
    Run ``spack spec -I py-dlio-profiler-py`` before installing to see what Spack is going
    to do.

----------

------------------------------
Build DLIO Profiler with CMake
------------------------------

Download the latest DLIO Profiler release from the Releases_ page or clone the develop
branch ('develop') from the DLIO Profiler repository
`https://github.com/hariharan-devarajan/dlio-profiler <https://github.com/hariharan-devarajan/dlio-profiler>`_.

Build DLIO Profiler Dependencies
********************************

The main dependencies DLIO Profiler are
1. cpp-logger : `https://github.com/hariharan-devarajan/cpp-logger.git <https://github.com/hariharan-devarajan/cpp-logger.git>`_ version: 0.0.1
2. gotcha: `https://github.com/LLNL/GOTCHA.git <https://github.com/LLNL/GOTCHA.git>`_ version: develop
3. brahma: `https://github.com/hariharan-devarajan/brahma.git <https://github.com/hariharan-devarajan/brahma.git>`_ version: 0.0.1

These dependencies can be either installed using spack or through cmake from respective respositories.

.. code-block:: Bash
    
    cmake . -B build -DCMAKE_INSTALL_PREFIX=<where you want to install DLIO Profiler>
    cmake --build build
    cmake --install build

-----------

.. explicit external hyperlink targets

.. _Releases: https://github.com/hariharan-devarajan/dlio-profiler/releases
.. _Spack: https://github.com/spack/spack
.. _Spack's shell support: https://spack.readthedocs.io/en/latest/getting_started.html#add-spack-to-the-shell
.. _packages.yaml: https://spack.readthedocs.io/en/latest/build_settings.html#external-packages
