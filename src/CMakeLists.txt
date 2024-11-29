cmake_minimum_required(VERSION 3.29..3.31)

add_library(${JACL_LIB} SHARED "tokenize.c" "tokens.c" "parse.c" "expr.c")
target_include_directories(${JACL_LIB} PUBLIC .)

add_executable(${JACL_BIN} "entry.c")

set_target_properties(
  ${JACL_BIN}
  PROPERTIES C_STANDARD 11
             C_STANDARD_REQUIRED TRUE
             LINKER_LANGUAGE C
             VERSION ${PROJECT_VERSION}
             RUNTIME_OUTPUT_DIRECTORY "../")

target_compile_options(${JACL_BIN} PRIVATE -g -Wall -Wextra -Wpedantic)
target_link_libraries(${JACL_BIN} PRIVATE ${PROJECT_NAME})
