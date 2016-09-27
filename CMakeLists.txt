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


#SET(WJRPC_CXX_STANDARD    "-std=c++11")
#SET(WJRPC_CXX_WARN_FLAGS  "-W -Wall -pedantic -Wextra")
#if ( NOT ${gcc_version_number} LESS 4008 )
#  SET(WJRPC_CXX_WARN_FLAGS    "${WJRPC_CXX_WARN_FLAGS} -ftemplate-backtrace-limit=0 ")
#endif()

#SET(CMAKE_CXX_FLAGS "${CXX_FLAGS} ${WJRPC_CXX_STANDARD} ${WJRPC_CXX_WARN_FLAGS} ${WJRPC_CXX_DEFINES}")
#SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2")

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
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /W3 /wd4503 /wd4307 /EHsc /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG  "/Od /D_DEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "/Yd /O2 /DNDEBUG")
else(UNIX)
endif(UNIX)


add_library(
  wjrpc
  wjrpc/incoming/incoming_holder.cpp
  wjrpc/outgoing/outgoing_holder.cpp
  wjrpc/incoming/send_error.cpp
)

if ( WJRPC_BUILD_TEST OR WJRPC_BUILD_ALL )
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

