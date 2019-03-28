set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})                                                                                                                 

add_executable(
  jsonrpc_test
  incoming_suite.cpp
  outgoing_suite.cpp
  method_suite.cpp
  handler_suite.cpp
  engine_suite.cpp
  ${FAS_TESTING_CPP}
  test_jsonrpc.cpp
)

target_link_libraries(jsonrpc_test wjrpc)

add_test(NAME jsonrpc_test COMMAND jsonrpc_test )
add_test_memcheck(jsonrpc_test)
        
if ( NOT CODE_COVERAGE )
  add_executable(benchmark1 benchmark.cpp)
endif()
