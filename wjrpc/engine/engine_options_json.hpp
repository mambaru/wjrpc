#pragma once
#include <wjrpc/types.hpp>

namespace wjrpc{
  
template<typename Options>
struct engine_options_json
{
  typedef Options options_type;

  /*JSON_NAME(allow_non_jsonrpc)*/
  JSON_NAME(call_lifetime_ms)
  JSON_NAME(remove_everytime)
  JSON_NAME(disable_handler_map)
  JSON_NAME(ignore_unknown_id)
  typedef wfc::json::object<
    options_type,
    ::wjson::member_list<
      /*::wjson::member< n_allow_non_jsonrpc,  options_type, bool,   &options_type::allow_non_jsonrpc>,*/
      ::wjson::member< n_call_lifetime_ms,   options_type, time_t, &options_type::call_lifetime_ms, wjson::time_interval_ms<> >,
      ::wjson::member< n_remove_everytime,   options_type, bool,   &options_type::remove_everytime>,
      ::wjson::member< n_ignore_unknown_id,   options_type, bool,   &options_type::ignore_unknown_id>,
      ::wjson::member< n_disable_handler_map,   options_type, bool,   &options_type::disable_handler_map>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};

}
