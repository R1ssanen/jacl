cmake_minimum_required(VERSION 3.28)
project(
  jacl
  VERSION 24.1.0
  DESCRIPTION "Just Another Compiled Language :-)"
  LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} "src/entry.c" "src/tokenize.c" "src/tokens.c"
                               "src/parse.c" "src/expr.c")

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES C_STANDARD 11
             C_STANDARD_REQUIRED TRUE
             LINKER_LANGUAGE C
             VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PRIVATE "src/")
target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra -Wpedantic
                                               -flto=auto)
target_link_libraries(${PROJECT_NAME} PRIVATE m)
