set_property(GLOBAL PROPERTY COVERAGE_FILES "")
function(dlp_add_test)
    add_test(${ARGV})
    set_tests_properties(${ARGV0} PROPERTIES
            ENVIRONMENT "LLVM_PROFILE_FILE=${ARGV0}.profraw")
    get_property(OLD_CF GLOBAL PROPERTY COVERAGE_FILES)
    set(NEW_CF ${OLD_CF} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.profraw)
    set_property(GLOBAL PROPERTY COVERAGE_FILES ${NEW_CF})
endfunction()

add_executable(test_cpp cpp/test.cpp)
target_link_libraries(test_cpp ${PROJECT_NAME})
add_dependencies(test_cpp ${PROJECT_NAME})
add_dependencies(test_cpp ${PROJECT_NAME}_preload)

add_executable(test_c c/test.c)
target_link_libraries(test_c ${PROJECT_NAME})
add_dependencies(test_c ${PROJECT_NAME})
add_dependencies(test_c ${PROJECT_NAME}_preload)
function(set_common_properties test_name)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_LOG_LEVEL=DEBUG)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_TRACE_COMPRESSION=0)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_DATA_DIR=${CMAKE_CURRENT_BINARY_DIR}/data)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_LOG_FILE=${CMAKE_CURRENT_BINARY_DIR}/${test_name})
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_ENABLE=1)
endfunction()
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)

set(test_name test_cpp_basic_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/lib/libdlio_profiler_preload.so)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INIT=PRELOAD)

find_program(BASH_PROGRAM bash)
dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 32)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_cpp_basic_app_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data 1)
set_common_properties(${test_name})

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 32)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_cpp_basic_app_only_yaml)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ./data 1)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_CONFIGURATION=${CMAKE_CURRENT_SOURCE_DIR}/yaml/conf.yaml)



set(test_name test_cpp_basic_app_compress_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data 1)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_TRACE_COMPRESSION=1)


set(test_name test_cpp_basic_app_disable_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data 1)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_ENABLE=0)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file_not.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 0)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_c_basic_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_c ${CMAKE_CURRENT_BINARY_DIR}/data)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/lib/libdlio_profiler_preload.so)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INIT=PRELOAD)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 7)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_c_basic_app_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_c ${CMAKE_CURRENT_BINARY_DIR}/data 1)
set_common_properties(${test_name})

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 7)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_c_basic_app_disable_only)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_c ${CMAKE_CURRENT_BINARY_DIR}/data 1)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_ENABLE=0)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file_not.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 0)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_cpp_basic_meta)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/lib/libdlio_profiler_preload.so)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INIT=PRELOAD)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INC_METADATA=1)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 32)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(test_name test_cpp_basic_affinity)
dlp_add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/test_cpp ${CMAKE_CURRENT_BINARY_DIR}/data)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/lib/libdlio_profiler_preload.so)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INIT=PRELOAD)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INC_METADATA=1)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_SET_CORE_AFFINITY=1)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 32)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})


find_program(PYTHON_EXE python)
message("-- Found python at location " ${PYTHON_EXE})
set(test_name test_py_disable_only)
dlp_add_test(${test_name} ${PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/py/test.py)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_SOURCE_DIR}/venv/lib)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_ENABLE=0)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file_not.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 0)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

find_program(PYTHON_EXE python)
message("-- Found python at location " ${PYTHON_EXE})
set(test_name test_py_both)
dlp_add_test(${test_name} ${PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/py/test.py)
set_common_properties(${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_SOURCE_DIR}/venv/lib)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:${CMAKE_SOURCE_DIR}/dependency/.spack-env/view/lib64)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_LOG_FILE=${CMAKE_CURRENT_BINARY_DIR}/${test_name}_app)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/lib/libdlio_profiler_preload.so)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_DATA_DIR=${CMAKE_CURRENT_BINARY_DIR}/data)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_LOG_FILE=${CMAKE_CURRENT_BINARY_DIR}/${test_name})
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INIT=PRELOAD)
set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_INC_METADATA=1)

dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/check_file_at_least.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name}* 85)
set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})

set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/util.h)
if (ENABLE_DLIO_BENCHMARK_TESTS)
    add_subdirectory(dlio_benchmark)
endif ()

if (ENABLE_PAPER_TESTS)
    add_subdirectory(paper)
endif ()
add_subdirectory(dlp_analyzer)