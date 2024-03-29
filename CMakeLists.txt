cmake_minimum_required(VERSION 3.5)
# set(CMAKE_TOOLCHAIN_FILE cmake/arm-none-eabi-gcc.cmake)
project(GD32-F303CC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
enable_language(C ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
  set(CMAKE_CROSS_CONFIGS "Release;Debug")
  set(CMAKE_DEFAULT_BUILD_TYPE "Release")
  set(CMAKE_DEFAULT_CONFIGS "Release;Debug")
endif()

option(FORCE_COLORED_OUTPUT
       "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if(${FORCE_COLORED_OUTPUT})
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

# add_compile_options(
# 	-mcpu=cortex-m4
# 	-mthumb
#   -mfloat-abi=hard
# 	$<$<CONFIG:Debug>:-Og>
# 	$<$<CONFIG:Release>:-O3>
# 	$<$<CONFIG:MinSizeRel>:-Os>
# 	$<$<CONFIG:RelWithDebInfo>:-O3>
# )

# add_link_options(
# 	-mcpu=cortex-m4
# )

# add MCU build c flags
add_definitions(-DDEBUG -DATHENA_OV_LCOS)
add_definitions(-DGD32F30X_HD)
add_definitions(-DULOG_TEST)

add_subdirectory(Template)
