include_directories(../../wjson/)

add_executable(example1 example1.cpp)
target_link_libraries(example1 wjrpc)

add_executable(example2 example2.cpp)
target_link_libraries(example2 wjrpc)

add_executable(example3 example3.cpp)
target_link_libraries(example3 wjrpc)
