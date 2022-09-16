cmake_minimum_required(VERSION 3.5)

include(example_set_name)

file(
  GLOB
  FreeRTOSSrc
  ${PROJECT_SOURCE_DIR}/FreeRTOS/*.c
  ${PROJECT_SOURCE_DIR}/FreeRTOS/include/*.h
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/*.c
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/*.h
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/MemMang/*.c
  )

include_directories(
  . ${PROJECT_SOURCE_DIR}/FreeRTOS/include
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable ${PROJECT_SOURCE_DIR}/FreeRTOS
  ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM4F)

add_executable(${EXEC_NAME} gd32f30x_it.c main.c systick.c ${FreeRTOSSrc})

target_include_directories(${EXEC_NAME} PRIVATE .)

include(example_add_libs)
