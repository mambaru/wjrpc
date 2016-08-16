#pragma once

#include <iow/jsonrpc/errors/errors.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace iow{ namespace jsonrpc{

struct error_json
{
  JSON_NAME(code)
  JSON_NAME(message)

  typedef ::wjson::object<
    error,
    ::wjson::member_list<
      ::wjson::member<n_code,    error, error_code_t,      &error::code>,
      ::wjson::member<n_message, error, std::string,       &error::message>
    >
  > type;

  typedef error target; 
  typedef type::member_list member_list;
  typedef type::serializer  serializer;
};

}}
