set(test_name test_df_analyzer)
# df_add_test(${test_name}_only_pfw ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/df_analyzer/main.py
#         ${PROJECT_SOURCE_DIR}/df_analyzer/test.pfw -d --log-file ${CMAKE_BINARY_DIR}/test/df_analyzer/df_analyzer.log)
# df_add_test(${test_name}_only_pfw_gz ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/df_analyzer/main.py
#         ${PROJECT_SOURCE_DIR}/df_analyzer/test.pfw.gz -v --log-file ${CMAKE_BINARY_DIR}/test/df_analyzer/df_analyzer.log)
# df_add_test(${test_name}_both ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/df_analyzer/main.py
#         ${PROJECT_SOURCE_DIR}/df_analyzer/test.pfw* -d --log-file ${CMAKE_BINARY_DIR}/test/df_analyzer/df_analyzer.log)