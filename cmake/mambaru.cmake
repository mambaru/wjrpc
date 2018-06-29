if ( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
  message(STATUS "${PROJECT_NAME} is not top level project")
  return()
endif()

include(mambaopt)
include(mambalibs)

get_faslib()
get_mambaru(wjson WJSON_DIR "")

if (NOT WJRPC_DISABLE_LOG)
  get_mambaru(wlog WLOG_DIR WLOG_LIB)
else()
  add_definitions(-DWJRPC_DISABLE_LOG)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


