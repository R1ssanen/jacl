cmake_minimum_required(VERSION 3.29..3.31)

add_library(${JACL_LIB} SHARED
    tokenize.c
    tokens.c
    parse.c
    error.c 
    expr.c
    mem.c
    array.c
)

target_include_directories(${JACL_LIB} PUBLIC .)
target_link_libraries(${JACL_LIB} PRIVATE m)
