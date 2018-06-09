
add_executable(benchmark1 benchmark.cpp)

add_executable(
  test_jsonrpc
  incoming_suite.cpp
  outgoing_suite.cpp
  method_suite.cpp
  handler_suite.cpp
  engine_suite.cpp
  ${FAS_TESTING_CPP}
  test_jsonrpc.cpp
)

target_link_libraries(test_jsonrpc wjrpc)
add_test( test_jsonrpc test_jsonrpc)
