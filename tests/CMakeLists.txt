cmake_minimum_required(VERSION 3.29..3.31)

if(BUILD_TESTING)
  add_executable(${JACL_TEST_BIN} test.c)
  target_link_libraries(${JACL_TEST_BIN} PRIVATE ${JACL_LIB})

endif()
