add_library(
  ${EXEC_NAME}_lcos_board EXCLUDE_FROM_ALL
  at.c
  basicApp.c
  BoardInit.c
  cmd.c
  ovp921.c
  ovp921_flash.c
  ThreadFirstConsumer.c
  ThreadSecondConsumer.c
  ThreadUartEvent.c)

target_include_directories(${EXEC_NAME}_lcos_board PUBLIC .)

target_link_libraries(${EXEC_NAME}_lcos_board ${EXEC_NAME}_standard_peripherals
                      ${EXEC_NAME}_CMSIS ${EXEC_NAME}_periph_func)
