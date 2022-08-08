cmake_minimum_required(VERSION 3.5)

include(example_set_name)

file(
  GLOB
  FreeRTOSSrc
  ${PROJECT_SOURCE_DIR}/FreeRTOS/*.c
  ${PROJECT_SOURCE_DIR}/FreeRTOS/include/*.h
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/*.c
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/*.h)

include_directories(
  . ${PROJECT_SOURCE_DIR}/FreeRTOS/include
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable ${PROJECT_SOURCE_DIR}/FreeRTOS
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/RVDS/ARM_CM4F)

include_directories(${PROJECT_SOURCE_DIR}/ulog/src)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/utils/AsciiConvert)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/Include)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/CmdProtocol)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/simple_file)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol/Src/At_HashTable)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol/Src/At_internal)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/OV_LcosBoard)
include_directories(${PROJECT_SOURCE_DIR}/Utilities)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/core)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/include)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/rdc200a)

add_executable(${EXEC_NAME} gd32f30x_it.c main.c systick.c ${FreeRTOSSrc})

target_include_directories(${EXEC_NAME} PRIVATE .)

include(example_add_libs)

# target_include_directories(ulog PRIVATE .) target_link_libraries(${EXEC_NAME}
# ulog)

# target_include_directories(ATProtocol PRIVATE .)
# target_link_libraries(${EXEC_NAME} ATProtocol)

# target_include_directories(CmdProtocol PRIVATE .)
# target_link_libraries(${EXEC_NAME} CmdProtocol)

# target_include_directories(AsciiConvert PRIVATE .)
# target_link_libraries(${EXEC_NAME} AsciiConvert)
