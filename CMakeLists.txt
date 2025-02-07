cmake_minimum_required(VERSION 3.29..3.31)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
project(
  jacl
  VERSION 24.1.1
  DESCRIPTION "Just Another Compiled Language"
  LANGUAGES C)

set(JACL_LIB ${PROJECT_NAME})
add_subdirectory(src)

set(JACL_BIN jaclang)
add_executable(${JACL_BIN} entry.c)

set_target_properties(
  ${JACL_BIN}
  PROPERTIES C_STANDARD 11
             C_STANDARD_REQUIRED TRUE
             LINKER_LANGUAGE C
             VERSION ${PROJECT_VERSION})

target_compile_options(${JACL_BIN} PRIVATE -g -Wall -Wextra -Wpedantic)
target_link_libraries(${JACL_BIN} PRIVATE ${PROJECT_NAME})

