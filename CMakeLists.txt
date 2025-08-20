cmake_minimum_required(VERSION 3.30)
project(jac LANGUAGES C)

add_executable(jac
    "src/main.c"
    "src/lex.c"
    "src/ast.c"
    "src/gen.c"
)

target_include_directories(jac PRIVATE "include")

set_target_properties(jac PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
    C_EXTENSIONS DISABLED
    EXPORT_COMPILE_COMMANDS TRUE
)

set(CMAKE_C_FLAGS_RELEASE -O3)
set(CMAKE_C_FLAGS_DEBUG -g)
target_compile_options(jac PRIVATE -Wall -pedantic -march=native )
#target_link_options(jac PRIVATE -fsanitize=address)
