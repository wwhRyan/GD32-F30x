add_library(
  ${EXEC_NAME}_vista_code EXCLUDE_FROM_ALL
  core/rti_vc_dload.c
  core/rti_vc_main.c
  core/rti_vc_panel.c
  core/rti_vc_rdc.c
  core/rti_vc_regio.c
  ./core/rdc200a/rdc200a.c
  ./core/rdc200a/rdc200a_flash.c)

target_include_directories(${EXEC_NAME}_vista_code PUBLIC include ./core
                                                          ./core/rdc200a)

include_directories(
  . ${PROJECT_SOURCE_DIR}/FreeRTOS/include
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable ${PROJECT_SOURCE_DIR}/FreeRTOS
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/RVDS/ARM_CM4F)

include_directories(${PROJECT_SOURCE_DIR}/ulog/src)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/utils/AsciiConvert)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/Include)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/CmdProtocol)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol/Src/At_HashTable)
include_directories(${PROJECT_SOURCE_DIR}/easy-mcu/AtProtocol/Src/At_internal)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/OV_LcosBoard)
include_directories(${PROJECT_SOURCE_DIR}/Utilities)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/core)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/include)
include_directories(${PROJECT_SOURCE_DIR}/BoardApp/raontech_lcos/libvc/rdc200a)

target_link_libraries(${EXEC_NAME}_vista_code)
