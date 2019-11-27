
add_executable(
  jsonrpc_test
  incoming_suite.cpp
  outgoing_suite.cpp
  method_suite.cpp
  handler_suite.cpp
  engine_suite.cpp
  test_jsonrpc.cpp
)

target_link_libraries(jsonrpc_test PUBLIC wjrpc fas::testing)

wci_test(TARGET jsonrpc_test COMMAND jsonrpc_test )
        
if ( NOT CODE_COVERAGE )
  add_executable(benchmark1 benchmark.cpp)
  wci_utils(benchmark1)
  target_link_libraries(benchmark1 PUBLIC wjrpc)
endif()
