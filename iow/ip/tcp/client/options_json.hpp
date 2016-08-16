#pragma once

#include <iow/io/client/options_json.hpp>
#include <iow/ip/tcp/client/options.hpp>
#include <iow/ip/tcp/connection/options_json.hpp>
#include <wjson/json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct options_json
{
  typedef ::iow::io::client::options_json<
    ::iow::ip::tcp::connection::options_json
  > base_json;
  
  typedef ::wjson::object<
    options,
    ::wjson::member_list<
      ::wjson::base< base_json >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}}

