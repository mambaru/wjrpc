include_directories(.)

add_executable(example01 example01.cpp)
target_link_libraries(example01 PRIVATE wjrpc wlog)

add_executable(example02 example02.cpp)
target_link_libraries(example02 PRIVATE wjrpc wlog)

add_executable(example03 example03.cpp)
target_link_libraries(example03 PRIVATE wjrpc wlog)

add_executable(example2 example2.cpp)
target_link_libraries(example2 PRIVATE wjrpc wlog)

add_executable(example3 example3.cpp calc/calc1.cpp)
target_link_libraries(example3 PRIVATE wjrpc wlog)

add_executable(example4 example4.cpp calc/calc1.cpp)
target_link_libraries(example4 PRIVATE wjrpc wlog)

add_executable(example5 example5.cpp calc/calc1.cpp)
target_link_libraries(example5 PRIVATE wjrpc wlog)

add_executable(example6 example6.cpp calc/calc1.cpp)
target_link_libraries(example6 PRIVATE wjrpc wlog)

add_executable(example7 example7.cpp calc/calc1.cpp)
target_link_libraries(example7 PRIVATE wjrpc wlog)

add_executable(example8 example8.cpp calc/calc1.cpp)
target_link_libraries(example8 PRIVATE wjrpc wlog)

add_executable(example9 example9.cpp calc/calc1.cpp)
target_link_libraries(example9 PRIVATE wjrpc wlog)

add_executable(example10 example10.cpp calc/calc1.cpp)
target_link_libraries(example10 PRIVATE wjrpc wlog)

add_executable(example11 example11.cpp calc/calc1.cpp calc/calc_p.cpp)
target_link_libraries(example11 PRIVATE wjrpc wlog)

add_executable(calc_proxy calc_proxy.cpp calc/calc_p.cpp calc/calc1.cpp calc/pserver.cpp calc/pclient.cpp)
target_link_libraries(calc_proxy PRIVATE wjrpc wlog)

wci_utils(
  example01 example02 example03 example2 example3 example4 
  example5  example6  example7  example8 example9 example10 
  example11 calc_proxy)
