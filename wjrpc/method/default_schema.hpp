//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2019
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <vector>
#include <wjson/wjson.hpp>

namespace wjrpc{

struct default_schema
{
  std::string name;
  std::string params;
  std::string result;
  std::string error;
  
  typedef std::vector<default_schema> schema_list_t;
  struct json
  {
    JSON_NAME(name)
    JSON_NAME(params)
    JSON_NAME(result)
    JSON_NAME(error)
    typedef wjson::object<
      default_schema,
      wjson::member_list<
        wjson::member<n_name,   default_schema, std::string, &default_schema::name, wjson::value<std::string> >,
        wjson::member<n_params, default_schema, std::string, &default_schema::params, wjson::raw_value<> >,
        wjson::member<n_result, default_schema, std::string, &default_schema::result, wjson::raw_value<> >,
        wjson::member<n_error,  default_schema, std::string, &default_schema::error, wjson::raw_value<> >
      >
    > type;
    typedef type::target target;
    typedef type::member_list member_list;
    typedef type::serializer serializer;
    
    typedef wjson::vector_of<type> list;
  };
};

}
