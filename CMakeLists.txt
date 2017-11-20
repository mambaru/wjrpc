cmake_minimum_required(VERSION 2.6)

project(wjrpc)

include_directories(.)
include_directories(../wjson/)
include_directories(../faslib/)


if ( NOT FAS_TESTING_CPP )
  set( FAS_TESTING_CPP ${CMAKE_SOURCE_DIR}/../faslib/fas/testing/testing.cpp )
  include_directories( ${CMAKE_SOURCE_DIR} )
  include_directories( ${CMAKE_SOURCE_DIR}/../faslib )
endif()

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror -pedantic -fno-rtti -std=c++11")
  message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
endif(UNIX)


add_library(
  wjrpc
  wjrpc/engine/call_map.cpp
  wjrpc/incoming/incoming_holder.cpp
  wjrpc/outgoing/outgoing_holder.cpp
  wjrpc/incoming/send_error.cpp
)

if ( BUILD_TESTING )
  if(UNIX AND PARANOID_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align \
                         -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization\
                         -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
                         -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
                         -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
                         -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
  endif()
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()
