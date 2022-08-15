add_library(
  ${EXEC_NAME}_vista_code EXCLUDE_FROM_ALL
  core/rti_vc_dload.c
  core/rti_vc_main.c
  core/rti_vc_panel.c
  core/rti_vc_rdc.c
  core/rti_vc_regio.c
  ./core/rdc200a/rdc200a.c
  ./core/rdc200a/rdc200a_flash.c
  port/rti_vc_delay.c
  port/rti_vc_devif.c)

target_include_directories(${EXEC_NAME}_vista_code PUBLIC include ./core
                                                          ./core/rdc200a)

target_link_libraries(${EXEC_NAME}_vista_code
                      PRIVATE ${EXEC_NAME}_raontech_lcos)
