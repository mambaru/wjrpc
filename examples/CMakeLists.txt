include_directories(.)

add_executable(example01 example01.cpp)
target_link_libraries(example01 wjrpc)

add_executable(example2 example2.cpp)
target_link_libraries(example2 wjrpc)

add_executable(example3 example3.cpp calc/calc1.cpp)
target_link_libraries(example3 wjrpc)

add_executable(example4 example4.cpp calc/calc1.cpp)
target_link_libraries(example4 wjrpc)

add_executable(example5 example5.cpp calc/calc1.cpp)
target_link_libraries(example5 wjrpc)

add_executable(example6 example6.cpp calc/calc1.cpp)
target_link_libraries(example6 wjrpc)

add_executable(example7 example7.cpp calc/calc1.cpp)
target_link_libraries(example7 wjrpc)

add_executable(example8 example8.cpp calc/calc1.cpp)
target_link_libraries(example8 wjrpc)

add_executable(example9 example9.cpp calc/calc1.cpp)
target_link_libraries(example9 wjrpc)

add_executable(example10 example10.cpp calc/calc1.cpp)
target_link_libraries(example10 wjrpc)

add_executable(example11 example11.cpp calc/calc1.cpp calc/calc_p.cpp)
target_link_libraries(example11 wjrpc)

add_executable(calc_proxy calc_proxy.cpp calc/calc_p.cpp calc/calc1.cpp calc/pserver.cpp calc/pclient.cpp)
target_link_libraries(calc_proxy wjrpc)
