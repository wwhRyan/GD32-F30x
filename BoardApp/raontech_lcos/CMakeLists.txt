add_subdirectory(libvc)

add_library(
  ${EXEC_NAME}_raontech_lcos EXCLUDE_FROM_ALL
  basicApp.c
  BoardInit.c
  ThreadFirstConsumer.c
  ThreadSecondConsumer.c
  ThreadUartEvent.c
  at.c
  at_cmd/AtCmdHashTable.c
  at_cmd/AtKeyHashTable.c)

target_include_directories(${EXEC_NAME}_raontech_lcos PUBLIC . ./at_cmd)

target_link_libraries(
  ${EXEC_NAME}_raontech_lcos
  PRIVATE ${EXEC_NAME}_standard_peripherals
          ${EXEC_NAME}_CMSIS
          ${EXEC_NAME}_periph_func
          ${EXEC_NAME}_vista_code
          ${EXEC_NAME}_utils
          ATProtocol
          CmdProtocol
          simple_file
          AsciiConvert
          ulog
          utils)
