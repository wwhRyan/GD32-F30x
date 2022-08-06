add_subdirectory(${PROJECT_SOURCE_DIR}/Firmware Firmware)
add_subdirectory(${PROJECT_SOURCE_DIR}/Utilities Utilities)
add_subdirectory(${PROJECT_SOURCE_DIR}/BoardApp BoardApp)
add_subdirectory(${PROJECT_SOURCE_DIR}/PeriphFunc PeriphFunc)
add_subdirectory(${PROJECT_SOURCE_DIR}/ulog ulog)

target_include_directories(${EXEC_NAME}_CMSIS PRIVATE .)
target_include_directories(${EXEC_NAME}_standard_peripherals PRIVATE .)
target_include_directories(${EXEC_NAME}_periph_func PRIVATE .)
target_include_directories(${EXEC_NAME}_utils PRIVATE .)
target_include_directories(${EXEC_NAME}_raontech_lcos PRIVATE .)
target_include_directories(${EXEC_NAME}_vista_code PRIVATE .)

target_link_libraries(
  ${EXEC_NAME}
  ${EXEC_NAME}_CMSIS
  ${EXEC_NAME}_standard_peripherals
  ${EXEC_NAME}_periph_func
  ${EXEC_NAME}_utils
  ${EXEC_NAME}_lcos_board
  ${EXEC_NAME}_vista_code)
