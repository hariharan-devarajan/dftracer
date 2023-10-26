find_program(PYTHON_EXE python)
message("-- Found python at location " ${PYTHON_EXE})
set(test_name test_dlp_analyzer)
dlp_add_test(${test_name}_only_pfw ${PYTHON_EXE} ${PROJECT_SOURCE_DIR}/dlp_analyzer/main.py
        ${PROJECT_SOURCE_DIR}/dlp_analyzer/test.pfw -d --log-file ${CMAKE_BINARY_DIR}/test/dlp_analyzer/dlp_analyzer.log)
dlp_add_test(${test_name}_only_pfw_gz ${PYTHON_EXE} ${PROJECT_SOURCE_DIR}/dlp_analyzer/main.py
        /usr/workspace/iopp/dlio_paper_results/cosmoflow-profile-compress-meta-scale-all-512/.trace*.pfw.gz -v --log-file ${CMAKE_BINARY_DIR}/test/dlp_analyzer/dlp_analyzer.log)
dlp_add_test(${test_name}_both ${PYTHON_EXE} ${PROJECT_SOURCE_DIR}/dlp_analyzer/main.py
        ${PROJECT_SOURCE_DIR}/dlp_analyzer/test.pfw* -d --log-file ${CMAKE_BINARY_DIR}/test/dlp_analyzer/dlp_analyzer.log)