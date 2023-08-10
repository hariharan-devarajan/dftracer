import os
import re
import subprocess
import sys
from pathlib import Path
print(sys.argv)
from setuptools import Extension, setup, find_namespace_packages
from setuptools.command.build_ext import build_ext
import site

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        cmake_args = []
        install_prefix = sys.prefix
        if "VIRTUAL_ENV" in os.environ:
            install_prefix = os.environ['VIRTUAL_ENV']
        elif "CONDA_DEFAULT_ENV" in os.environ:
            install_prefix = os.environ['CONDA_DEFAULT_ENV']
        if "DLIO_LOGGER_USER" in os.environ:
            install_prefix=site.USER_BASE
            cmake_args += [f"-DUSER_INSTALL=ON"]
        cmake_args += [f"-DCMAKE_INSTALL_PREFIX={install_prefix}"]
        project_dir = Path.cwd()
        dependency_file = open(f"{project_dir}/dependency/cpp.requirements.txt", 'r')
        dependencies = dependency_file.readlines()
        for dependency in dependencies:
            parts = dependency.split(",")
            clone_dir = f"{project_dir}/dependency/{parts[0]}"
            need_install = parts[3]
            print(f"Installing {parts[0]} into {install_prefix}")
            os.system(f"bash {project_dir}/dependency/install_dependency.sh {parts[1]} {clone_dir} {install_prefix} {parts[2]} {need_install}")
        cmake_args += [f"-DCMAKE_PREFIX_PATH={install_prefix}"]
        print(cmake_args)
        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = project_dir / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.parent.resolve()
        sourcedir = extdir.parent.resolve()
        print(f"{extdir}")
        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"

        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.

        #    f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}/lib",
        #    f"-DPYTHON_EXECUTABLE={sys.executable}",
        #    f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
        #]
        build_args = []
        # Adding CMake arguments set as environment variable
        # (needed e.g. to build for ARM OSx on conda-forge)
        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

        # In this example, we pass in the version to C++. You might not need to.
        cmake_args += [f"-DEXAMPLE_VERSION_INFO={self.distribution.get_version()}"]


        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += [f"-j{self.parallel}"]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)
        print("cmake", ext.sourcedir, cmake_args)
        subprocess.run(
            ["cmake", ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        subprocess.run(
            ["cmake", "--build", ".", *build_args], cwd=build_temp, check=True
        )
        subprocess.run(
            ["cmake", "--install", "."], cwd=build_temp, check=True
        )

import pathlib
here = pathlib.Path(__file__).parent.resolve()
long_description = (here / "README.md").read_text(encoding="utf-8")
# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    name="dlio_profiler_py",
    version="0.0.1",
    description="I/O profiler for deep learning python apps. Specifically for dlio_benchmark.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/hariharan-devarajan/dlio-profiler",
    author="Hariharan Devarajan (Hari)",
    email="mani.hariharan@gmail.com",
    classifiers=[  # Optional
        # How mature is this project? Common values are
        #   3 - Alpha
        #   4 - Beta
        #   5 - Production/Stable
        "Development Status :: 3 - Alpha",
        # Indicate who your project is intended for
        "Intended Audience :: HPC",
        "Topic :: Software Development :: Build Tools",
        # Pick your license as you wish
        "License :: OSI Approved :: MIT License",
        # Specify the Python versions you support here. In particular, ensure
        # that you indicate you support Python 3. These classifiers are *not*
        # checked by 'pip install'. See instead 'python_requires' below.
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3 :: Only",
    ],
    install_requires=["pybind11[global]"],
    keywords="profiler, deep learning, I/O, benchmark, NPZ, pytorch benchmark, tensorflow benchmark",
    project_urls={  # Optional
        "Bug Reports": "https://github.com/hariharan-devarajan/dlio-profiler/issues",
        "Source": "https://github.com/hariharan-devarajan/dlio-profiler",
    },
    packages=find_namespace_packages(where="."),
    package_dir={"dlio_profiler": "dlio_profiler"},
    ext_modules=[CMakeExtension("dlio_profiler_py")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    extras_require={"test": ["pytest>=6.0"]},
    python_requires=">=3.7",
)
