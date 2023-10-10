find_program(DLIO_BENCHMARK_EXE dlio_benchmark)
message("-- Found dlio_benchmark at location " ${DLIO_BENCHMARK_EXE})

set(COMMON_ARGS workload=unet3d ++workload.train.computation_time=0.05
        ++workload.evaluation.eval_time=0.01 ++workload.train.epochs=2 ++workload.workflow.train=True
        ++workload.workflow.generate_data=True ++workload.dataset.num_files_train=16
        ++workload.dataset.num_files_eval=16 ++workload.reader.read_threads=2
        ++workload.dataset.record_length=4096 ++workload.dataset.record_length_stdev=0
        ++workload.framework=tensorflow ++workload.reader.data_loader=tensorflow)
set(formats npz hdf5 tfrecord png jpeg)
set(lines 1728 192 416 544 640)
list(LENGTH formats formats_length)
math(EXPR formats_length "${formats_length} - 1")
message(STATUS "Number of formats: ${formats_length}")
foreach (index RANGE ${formats_length})
    list(GET formats index format)
    list(GET lines index line)
    set(test_name test_dlio_${format})
    dlp_add_test(${test_name} ${DLIO_BENCHMARK_EXE} ${COMMON_ARGS} ++workload.dataset.format=${format}
            ++workload.output.folder=${CMAKE_CURRENT_BINARY_DIR}/output_${format})
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_ENABLE=1)
    set_property(TEST ${test_name} APPEND PROPERTY ENVIRONMENT DLIO_PROFILER_LOG_LEVEL=ERROR)

    dlp_add_test(check_file_exists_${test_name} ${BASH_PROGRAM} ${CMAKE_SOURCE_DIR}/test/check_file.sh ${CMAKE_CURRENT_BINARY_DIR}/output_${format}/.trace*.pfw ${line})
    set_tests_properties(check_file_exists_${test_name} PROPERTIES DEPENDS ${test_name})
endforeach ()