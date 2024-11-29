cmake_minimum_required(VERSION 3.29..3.31)
project(
  jacl
  VERSION 24.1.0
  DESCRIPTION "Just Another Compiled Language"
  LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(JACL_LIB ${PROJECT_NAME})
set(JACL_BIN jaclang)
set(JACL_TEST_BIN ${PROJECT_NAME}_tests)

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

add_test(NAME JaclangUnitTests COMMAND ${JACL_TEST_BIN})
