add_library(${EXEC_NAME}_utils EXCLUDE_FROM_ALL gd32f307c_eval.c utils.c)

target_include_directories(${EXEC_NAME}_utils PUBLIC .)

target_link_libraries(
  ${EXEC_NAME}_utils
  ${EXEC_NAME}_periph_func
  ${EXEC_NAME}_standard_peripherals
  ${EXEC_NAME}_CMSIS
  ${EXEC_NAME}_raontech_lcos
  simple_file
  ulog)
