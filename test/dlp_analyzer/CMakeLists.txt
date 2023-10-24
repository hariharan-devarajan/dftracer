find_program(PYTHON_EXE python)
message("-- Found python at location " ${PYTHON_EXE})
set(test_name test_dlp_analyzer)
dlp_add_test(${test_name} ${PYTHON_EXE} ${PROJECT_SOURCE_DIR}/dlp_analyzer/main.py
        ${PROJECT_SOURCE_DIR}/dlp_analyzer/test.pfw* -d --log-file ${CMAKE_BINARY_DIR}/test/dlp_analyzer/dlp_analyzer.log)