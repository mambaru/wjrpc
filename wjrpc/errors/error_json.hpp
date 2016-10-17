//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/errors/errors.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wjrpc{

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

}
