add_library(
  ${EXEC_NAME}_periph_func EXCLUDE_FROM_ALL
  adc.c
  dac.c
  eeprom.c
  i2c.c
  port.c
  timer.c
  uart.c)

target_include_directories(${EXEC_NAME}_periph_func PUBLIC .)

target_link_libraries(${EXEC_NAME}_periph_func
                      ${EXEC_NAME}_standard_peripherals ${EXEC_NAME}_CMSIS)
