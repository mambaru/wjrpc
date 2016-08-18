#pragma once

#include <string>
#include <iow/io/client/options.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace iow{ namespace io{ namespace client{

template< typename ConnectionOptionsJson >
struct options_json
{
  typedef ConnectionOptionsJson connection_json;
  typedef typename connection_json::target connection_options;
  typedef options<connection_options> option_type;
  
  JSON_NAME(async_connect)
  JSON_NAME(addr)
  JSON_NAME(port)
  JSON_NAME(threads)
  JSON_NAME(connection)
  JSON_NAME(reconnect_timeout_ms)
  JSON_NAME(connect_count)
  
  typedef ::wjson::object<
    option_type,
    ::wjson::member_list<
      ::wjson::member< n_connection, option_type, connection_options, &option_type::connection, connection_json>,
      ::wjson::member< n_async_connect, option_type, bool, &option_type::async_connect>,
      ::wjson::member< n_addr, option_type, std::string, &option_type::addr>,
      ::wjson::member< n_port, option_type, std::string, &option_type::port>,
      ::wjson::member< n_threads, option_type, int, &option_type::threads>,
      ::wjson::member< n_reconnect_timeout_ms, option_type, time_t, &option_type::reconnect_timeout_ms>,
      ::wjson::member<n_connect_count, option_type, int, &option_type::connect_count>
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}
