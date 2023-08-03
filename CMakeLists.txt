cmake_minimum_required(VERSION 3.10)

project(wjrpc)

include(cmake/ci.cmake)

wci_submodule(NAME faslib)
wci_submodule(NAME wjson)
wci_submodule(NAME wlog)

add_library(
  wjrpc
  wjrpc/engine/call_map.cpp
  wjrpc/incoming/incoming_holder.cpp
  wjrpc/outgoing/outgoing_holder.cpp
  wjrpc/incoming/send_error.cpp
)

if ( CMAKE_CXX_STANDARD EQUAL 20 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  update_ogenc(TARGETS wjrpc WARNINGS -Wzero-as-null-pointer-constant OFF)
endif()

wci_targets(wjrpc)
target_include_directories(wjrpc PUBLIC ${wjrpc_SOURCE_DIR})
target_link_libraries(wjrpc PUBLIC wlog)


if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()

set(CPACK_PACKAGE_NAME "wjrpc")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "4")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

install(DIRECTORY wjrpc DESTINATION include FILES_MATCHING PATTERN "*.hpp" )
install(TARGETS wjrpc LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
include(CPack)
