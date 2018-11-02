
add_executable(benchmark1 benchmark.cpp)

#message(STATUS "-->: ${FAS_TESTING_CPP}")
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

target_link_libraries(jsonrpc_test wjrpc wlog)


add_test(NAME jsonrpc_test COMMAND jsonrpc_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
add_test(NAME jsonrpc_test-memcheck COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/memcheck.sh ${CMAKE_CURRENT_BINARY_DIR}/jsonrpc_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
