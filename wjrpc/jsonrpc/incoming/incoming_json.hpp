#pragma once

#include <iow/jsonrpc/incoming/incoming.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>


namespace iow{ namespace jsonrpc{

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef ::wjson::iterator_pair<pair_type> pair_json;

  JSON_NAME(id)
  JSON_NAME(method)
  JSON_NAME(params)
  JSON_NAME(result)
  JSON_NAME(error)

  typedef ::wjson::object<
    incoming, 
    ::wjson::member_list<
      ::wjson::member<n_method, incoming, pair_type, &incoming::method, pair_json>,
      ::wjson::member<n_params, incoming, pair_type, &incoming::params, pair_json>,
      ::wjson::member<n_result, incoming, pair_type, &incoming::result, pair_json>,
      ::wjson::member<n_error,  incoming, pair_type, &incoming::error,  pair_json>,
      ::wjson::member<n_id,     incoming, pair_type, &incoming::id,     pair_json>
    >
  > type;

  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer  serializer;

};

  
}}
