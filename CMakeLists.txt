cmake_minimum_required(VERSION 3.4...3.18)
#------------------------------------------------------------------------------
# Version information
#------------------------------------------------------------------------------

set(DLIO_PROFILER_VERSION_MAJOR "2")
set(DLIO_PROFILER_VERSION_MINOR "0")
set(DLIO_PROFILER_VERSION_PATCH "1")
set(DLIO_PROFILER_PACKAGE ${PROJECT_NAME})
set(DLIO_PROFILER_PACKAGE_NAME ${PROJECT_NAME})
set(DLIO_PROFILER_PACKAGE_VERSION "${DLIO_PROFILER_VERSION_MAJOR}.${DLIO_PROFILER_VERSION_MINOR}.${DLIO_PROFILER_VERSION_PATCH}")
set(DLIO_PROFILER_PACKAGE_VERSION_MAJOR "${DLIO_PROFILER_VERSION_MAJOR}.${DLIO_PROFILER_VERSION_MINOR}")
set(DLIO_PROFILER_PACKAGE_VERSION_MINOR "${DLIO_PROFILER_VERSION_PATCH}")
set(DLIO_PROFILER_PACKAGE_STRING "${DLIO_PROFILER_PACKAGE_NAME} ${DLIO_PROFILER_PACKAGE_VERSION}")
set(DLIO_PROFILER_PACKAGE_TARNAME "${DLIO_PROFILER_PACKAGE}")

project(dlio_profiler LANGUAGES C CXX)


# Convenience defines
string(TOUPPER "${PROJECT_NAME}" UPPER_PROJECT_NAME)
string(TOLOWER "${PROJECT_NAME}" LOWER_PROJECT_NAME)

#------------------------------------------------------------------------------
# Internal Includes for header and libraries
#------------------------------------------------------------------------------
# Get installation directories -- these get used in various places;
# best to just make them available
option (DLIO_PROFILER_LIBDIR_AS_LIB OFF)
if (NOT DLIO_PROFILER_LIBDIR_AS_LIB)
    include(GNUInstallDirs)
endif ()
#
if (CMAKE_INSTALL_LIBDIR)
   message(STATUS "CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")
   set(DLIO_PROFILER_LIBDIR ${CMAKE_INSTALL_LIBDIR})
   set(DLIO_PROFILER_INSTALL_INCLUDE_DIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
   set(DLIO_PROFILER_INSTALL_DOCDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR})
   set(DLIO_PROFILER_INSTALL_SYSCONFDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/modulefiles)
else ()
   set(DLIO_PROFILER_LIBDIR "lib")
   set(DLIO_PROFILER_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
   set(DLIO_PROFILER_INSTALL_DOCDIR "${CMAKE_INSTALL_PREFIX}/doc")
   set(DLIO_PROFILER_INSTALL_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc/modulefiles")
   message(STATUS "DLIO_PROFILER_LIBDIR set to ${DLIO_PROFILER_LIBDIR}")
endif ()

#------------------------------------------------------------------------------
# Internal Paths for cmake libraries and Setup install and output Directories
#------------------------------------------------------------------------------
# This sets where to look for dependent libraries
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_BINARY_DIR} ${CMAKE_INSTALL_PREFIX})
# This sets where to look for dependent library's cmake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/${DLIO_PROFILER_LIBDIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/share/cmake)

#------------------------------------------------------------------------------
if (NOT DLIO_PROFILER_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
    set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include CACHE PATH "Store the headers.")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DLIO_PROFILER_LIBDIR} CACHE PATH "Single Directory for all Libraries")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DLIO_PROFILER_LIBDIR} CACHE PATH "Single Directory for all static libraries.")
endif ()

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/${DLIO_PROFILER_LIBDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects.
#-----------------------------------------------------------------------------
if (NOT DLIO_PROFILER_EXPORTED_TARGETS)
    set(DLIO_PROFILER_EXPORTED_TARGETS "dlio_profiler-targets")
endif ()


include(dlio_profiler-utils)
#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
# option(ENABLE_DLIO_PROFILER_DEBUG "Include debugging prints and logging" OFF) # This is not needed as we have CMAKE_BUILD_TYPE
# This is verbose, maybe an alternate might help simplify
#option(BUILD_URPC "Build DLIO_PROFILER's URPC code" OFF)
#option(ENABLE_PERFFLOW "Build with PerfFlow Aspect support" OFF)
#option(ENABLE_UCX_DTL "Build DLIO_PROFILER's UCX data transport layer" OFF)

# This option is off by default because non-developers should not use
# this option under normal circumstances.
option(DLIO_PROFILER_WARNINGS_AS_ERRORS "Build with warnings promoted to errors." OFF)
mark_as_advanced(DLIO_PROFILER_WARNINGS_AS_ERRORS)

option (DLIO_PROFILER_USE_CLANG_LIBCXX OFF)
if (DLIO_PROFILER_USE_CLANG_LIBCXX)
    set (GCC_TOOLCHAIN "")
endif (DLIO_PROFILER_USE_CLANG_LIBCXX)

# Options
option (DLIO_PROFILER_DISABLE_HWLOC "Disable HWLOC" On)
if (NOT DLIO_PROFILER_DISABLE_HWLOC) 
    set(DLIO_PROFILER_HWLOC_ENABLE 1)
endif()
option (DLIO_PROFILER_BUILD_PYTHON_BINDINGS "Build python bindings." ON)
set(DLIO_PROFILER_PYTHON_EXE "python3" CACHE STRING "Python executable to use for building.")
set(DLIO_PROFILER_PYTHON_SITE $ENV{DLIO_PROFILER_PYTHON_SITE} CACHE STRING "Python site packages to use for building.")


if (DLIO_PROFILER_BUILD_PYTHON_BINDINGS)
    if (DLIO_PROFILER_PYTHON_SITE)
        set(PYTHON_SITE_PACKAGES "${DLIO_PROFILER_PYTHON_SITE}")
        set(MAIN_PYTHON_SITE_PACKAGES ${PYTHON_SITE_PACKAGES})
    else()
        set(PYTHON_SITE_PACKAGES ${CMAKE_INSTALL_PREFIX})
        execute_process (COMMAND ${DLIO_PROFILER_PYTHON_EXE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE MAIN_PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    include_directories(${PYTHON_SITE_PACKAGES}/dlio_profiler/include)
    link_directories(${PYTHON_SITE_PACKAGES}/dlio_profiler/lib)
endif()

# install dependency
option (DLIO_PROFILER_INSTALL_DEPENDENCIES "Install DLIO Profiler dependencies" OFF)

# testing
option (DLIO_PROFILER_ENABLE_TESTS "Enable tests for DLIO_PROFILER." OFF)
option (DLIO_PROFILER_ENABLE_DLIO_BENCHMARK_TESTS "Enable dlio_benchmark tests" OFF)
option (DLIO_PROFILER_ENABLE_PAPER_TESTS "Enable paper tests" OFF)

#------------------------------------------------------------------------------
# Compiler setup
#------------------------------------------------------------------------------
include(ExternalProject)
# It requires include(GNUInstallDirs) before if that is needed
include(SetupCompiler)

#------------------------------------------------------------------------------
# Build Type based configurations
#------------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release") # Setting default as Release
endif()

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif ()
#------------------------------------------------------------------------------
# Disallow in-source build
#------------------------------------------------------------------------------
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR
            "${PROJECT_NAME} requires an out of source Build. "
            "Please create a separate binary directory and run CMake there.")
endif ()
#------------------------------------------------------------------------------
# Set a default build type if none was specified
#------------------------------------------------------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
            "MinSizeRel" "RelWithDebInfo")
endif ()
#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

if (DLIO_PROFILER_INSTALL_DEPENDENCIES) 
    message(STATUS "[${PROJECT_NAME}] downloading dependencies. Please run make for downloading depedencies and then do reconfigure without dependency flag.")
    message(STATUS "[${PROJECT_NAME}] Installing depedencies at ${CMAKE_INSTALL_PREFIX}")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependency)
else()
# Required Dependencies
# =============================================================================
set(DEPENDENCY_LIB "")
find_package(cpp-logger REQUIRED)
if (${CPP_LOGGER_FOUND})
    include_directories(${CPP_LOGGER_INCLUDE_DIRS})
    set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${CPP_LOGGER_LIBRARIES})
    message(STATUS "[CPP_LOGGER] found cpp-logger at ${CPP_LOGGER_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "-- [CPP_LOGGER] is needed for ${PROJECT_NAME} build")
endif ()
find_package(brahma REQUIRED)
if (${brahma_FOUND})
    message(STATUS "[BRAHMA] found brahma at ${BRAHMA_INCLUDE_DIRS}")
    include_directories(${BRAHMA_INCLUDE_DIRS})
    set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${BRAHMA_LIBRARIES})
else ()
    message(FATAL_ERROR "-- [BRAHMA] is needed for ${PROJECT_NAME} build")
endif ()
find_package(yaml-cpp REQUIRED)
if (${yaml-cpp_FOUND})
    message(STATUS "[DLIO_PROFILER] found yaml-cpp at ${YAML_CPP_INCLUDE_DIR}")
    include_directories(${YAML_CPP_INCLUDE_DIR})
    set(YAML_CPP_LIBRARY_DIR "${YAML_CPP_CMAKE_DIR}/../../")
    set(DEPENDENCY_LIB ${DEPENDENCY_LIB} -L${YAML_CPP_LIBRARY_DIR} ${YAML_CPP_LIBRARIES})
else ()
    message(FATAL_ERROR "-- [DLIO_PROFILER] yaml-cpp is needed for ${PROJECT_NAME} build")
endif ()

# Optional Dependencies
# =============================================================================
if (DLIO_PROFILER_BUILD_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
    #add_subdirectory(dependency/py11bind)
endif()
if (NOT DLIO_PROFILER_DISABLE_HWLOC)
    find_package(HWLOC REQUIRED)
    if (${HWLOC_FOUND})
        include_directories(${HWLOC_INCLUDE_DIRS})
        set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${HWLOC_LIBRARIES})
        message(STATUS "[DLIO_PROFILER] found hwloc at ${HWLOC_INCLUDE_DIRS}")
    else ()
        message(FATAL_ERROR "-- [DLIO_PROFILER] hwloc is needed for ${PROJECT_NAME} build with -DDLIO_PROFILER_DISABLE_HWLOC-ON option.")
    endif ()
endif()




include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DLIO_PROFILER_PRELOAD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/dlio_profiler_preload.cpp)
set(DLIO_PROFILER_PRELOAD_PRIVATE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/dlio_logger_preload.h)

set(DLIO_PROFILER_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/brahma/posix.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/brahma/stdio.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/writer/chrome_writer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/utils/posix_internal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/dlio_profiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/core/dlio_profiler_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/utils/configuration_manager.cpp)
set(DLIO_PROFILER_CORE_PUBLIC_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/dlio_profiler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/core/constants.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/core/typedef.h)
set(DLIO_PROFILER_CORE_PRIVATE_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/utils/posix_internal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/utils/utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/brahma/posix.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/brahma/stdio.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/core/dlio_profiler_main.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/core/macro.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dlio_profiler/core/singleton.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/utils/configuration_manager.h)
add_library(${PROJECT_NAME} SHARED)
target_link_libraries(${PROJECT_NAME} ${DEPENDENCY_LIB})
add_library(${PROJECT_NAME}_preload SHARED)
target_link_libraries(${PROJECT_NAME}_preload ${DEPENDENCY_LIB} ${PROJECT_NAME})
if (DLIO_PROFILER_BUILD_PYTHON_BINDINGS)
    pybind11_add_module(${PROJECT_NAME}_py ${DLIO_PROFILER_CORE_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/src/dlio_profiler/dlio_profiler_py.cpp)
    target_compile_definitions(${PROJECT_NAME}_py PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
    target_link_libraries(${PROJECT_NAME}_py PRIVATE ${DEPENDENCY_LIB})
endif()
target_sources(${PROJECT_NAME}
        PRIVATE
        ${DLIO_PROFILER_CORE_SRC}
        )
target_sources(${PROJECT_NAME}_preload
        PRIVATE
        ${DLIO_PROFILER_PRELOAD_SRC}
        )
target_include_directories(${PROJECT_NAME}
        PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIB_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${PROJECT_NAME}
        MINSIZEREL_OUTPUT_NAME ${PROJECT_NAME}
        RELWITHDEBINFO_OUTPUT_NAME ${PROJECT_NAME}
        VERSION ${LIB_VERSION}
        SOVERSION ${LIB_VERSION}
        )

dlio_profiler_install_headers("${DLIO_PROFILER_CORE_PUBLIC_INCLUDE}")
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${DLIO_PROFILER_EXPORTED_TARGETS}
        LIBRARY DESTINATION ${DLIO_PROFILER_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${DLIO_PROFILER_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${DLIO_PROFILER_INSTALL_BIN_DIR}
)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS ${PROJECT_NAME}_preload
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    TARGETS
    ${PROJECT_NAME}_preload
    EXPORT
    ${DLIO_PROFILER_EXPORTED_TARGETS}
    LIBRARY DESTINATION ${DLIO_PROFILER_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${DLIO_PROFILER_INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${DLIO_PROFILER_INSTALL_BIN_DIR}
)
if (DLIO_PROFILER_BUILD_PYTHON_BINDINGS)
    install(TARGETS ${PROJECT_NAME}_py
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/symlink.sh CONTENT "echo -- Installing: symlink ${MAIN_PYTHON_SITE_PACKAGES}/$<TARGET_FILE_NAME:${PROJECT_NAME}_py> from ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}_py>;ln -sf ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}_py> ${MAIN_PYTHON_SITE_PACKAGES}/$<TARGET_FILE_NAME:${PROJECT_NAME}_py>")
    install(CODE "execute_process(
                COMMAND bash -c \"set -e
                chmod +x ${CMAKE_BINARY_DIR}/symlink.sh
                . ${CMAKE_BINARY_DIR}/symlink.sh
                \")")
endif()
#cmake_policy(SET CMP0079 NEW) # In case that we need more control over the target building order

if(DLIO_PROFILER_ENABLE_TESTS)
    message("-- " "[DLIO Profiler] Enabling Testing")
    if(CMAKE_BUILD_TYPE STREQUAL "PROFILE")
        message("-- " "[DLIO Profiler] building with CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
        set(CMAKE_C_FLAGS_PROFILE --coverage)
        set(CMAKE_CXX_FLAGS_PROFILE --coverage)
    endif()
    enable_testing()
    add_subdirectory(test)
    if(CMAKE_BUILD_TYPE STREQUAL "PROFILE")
        get_property(COVERAGE_FILE_LIST GLOBAL PROPERTY COVERAGE_FILES)
        add_custom_command(OUTPUT default.profdata COMMAND llvm-profdata merge -sparse ${COVERAGE_FILE_LIST} -o default.profdata VERBATIM)
        add_custom_target(merged_coverage DEPENDS default.profdata)
    endif()
endif()

#-----------------------------------------------------------------------------
# Configure the config.cmake file for the build directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.build.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake @ONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.install.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake @ONLY
)
install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake
        DESTINATION
        ${DLIO_PROFILER_LIBDIR}/cmake/${PROJECT_NAME}
)
#-----------------------------------------------------------------------------
# Configure the ${PROJECT_NAME}-config-version .cmake file for the install directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config-version.cmake.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake @ONLY
)

install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake
        DESTINATION
        ${DLIO_PROFILER_LIBDIR}/cmake/${PROJECT_NAME}
)

export(EXPORT ${DLIO_PROFILER_EXPORTED_TARGETS}
        FILE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake")

# Write the configure file
configure_file("${CMAKE_SOURCE_DIR}/cmake/configure_files/dlio_profiler_config.hpp.in"
               "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/dlio_profiler/dlio_profiler_config.hpp" @ONLY)


################################################################
# Install DLIO_PROFILER
################################################################

include(CMakePackageConfigHelpers)

# Write the version file. This is independent of build/install tree.
write_basic_package_version_file(
  DLIO_PROFILERConfigVersion.cmake
  VERSION "${DLIO_PROFILER_PACKAGE_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/dlio_profiler/dlio_profiler_config.hpp"
        DESTINATION
        ${DLIO_PROFILER_INSTALL_INCLUDE_DIR}/dlio_profiler)

install(EXPORT
        ${DLIO_PROFILER_EXPORTED_TARGETS}
        DESTINATION
        ${DLIO_PROFILER_LIBDIR}/cmake/${PROJECT_NAME}
        FILE
        ${DLIO_PROFILER_EXPORTED_TARGETS}.cmake
        )

# Install license and readme
install(FILES
        "${PROJECT_SOURCE_DIR}/LICENSE"
        "${PROJECT_SOURCE_DIR}/README.md"
        DESTINATION
        ${DLIO_PROFILER_INSTALL_DOCDIR})


###############################################################################
#                      Print out configuration summary
###############################################################################

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  set(DLIO_PROFILER_GNU_LINUX TRUE)
endif ()

# NOTE: message() outputs to stderr by default. We now use a string to
# maintain this information and then have cmake echo it to stdout. The
# only side effects are that if you use the CMake GUI, you won't see
# this output anymore (they only report stderr) and that if you add
# something to the list, you must remember your newline!
set(_str "\n== DLIO_PROFILER Configuration Summary ==\n\n")
string(APPEND _str "  PROJECT_SOURCE_DIR:   ${PROJECT_SOURCE_DIR}\n"
  "  PROJECT_BINARY_DIR:   ${PROJECT_BINARY_DIR}\n\n"
  "  CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}\n"
  "  CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}\n\n")
if (CMAKE_BUILD_TYPE MATCHES None)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}\n")
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}\n")
endif ()
string(APPEND _str "\n")

string(APPEND _str
  "  DLIO_PROFILER_PACKAGE_VERSION:        ${DLIO_PROFILER_PACKAGE_VERSION}\n")
string(APPEND _str
  "  DLIO_PROFILER_GIT_VERSION:            ${DLIO_PROFILER_GIT_VERSION}\n")
string(APPEND _str
  "  GLIBC_VERSION:               ${GLIBC_VERSION}\n")
  string(APPEND _str
  "  DLIO_PROFILER_PYTHON_EXE:               ${DLIO_PROFILER_PYTHON_EXE}\n")
  string(APPEND _str
  "  DLIO_PROFILER_PYTHON_SITE:               ${DLIO_PROFILER_PYTHON_SITE}\n")
  string(APPEND _str
  "  MAIN_PYTHON_SITE_PACKAGES:               ${MAIN_PYTHON_SITE_PACKAGES}\n")
  string(APPEND _str
  "  DEPENDENCY_LIB:               ${DEPENDENCY_LIB}\n")
#Print the true/false variables
append_str_tf(_str
  DLIO_PROFILER_GNU_LINUX
  DLIO_PROFILER_LIBDIR_AS_LIB
  DLIO_PROFILER_USE_CLANG_LIBCXX
  DLIO_PROFILER_WARNINGS_AS_ERRORS
  DLIO_PROFILER_BUILD_PYTHON_BINDINGS
  DLIO_PROFILER_ENABLE_TESTS
  DLIO_PROFILER_ENABLE_DLIO_BENCHMARK_TESTS
  DLIO_PROFILER_ENABLE_PAPER_TESTS
)
string(APPEND _str
  "\n== End DLIO_PROFILER Configuration Summary ==\n")

# Output to stdout
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${_str}")
set(_str)


###############################################################################
# Write a basic modulefile
###############################################################################

set(DLIO_PROFILER_MODULEFILE_NAME "dlio_profiler-${DLIO_PROFILER_PACKAGE_VERSION}.lua")
#  CACHE STRING
#  "The name of the DLIO_PROFILER modulefile to install. Must end in .lua.")

if (NOT (DLIO_PROFILER_MODULEFILE_NAME MATCHES ".+\.lua"))
  message(WARNING
    "DLIO_PROFILER_MODULEFILE_NAME must have extension \".lua\". Appending.")
  set(DLIO_PROFILER_MODULEFILE_NAME "${DLIO_PROFILER_MODULEFILE_NAME}.lua"
    CACHE STRING "" FORCE)
endif ()

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/configure_files/dlio_profiler_module.lua.in"
  "${CMAKE_BINARY_DIR}/dlio_profiler_module.lua.install"
  @ONLY)

install(FILES "${CMAKE_BINARY_DIR}/dlio_profiler_module.lua.install"
  RENAME "${DLIO_PROFILER_MODULEFILE_NAME}"
  DESTINATION
  "${DLIO_PROFILER_INSTALL_SYSCONFDIR}")

endif()
