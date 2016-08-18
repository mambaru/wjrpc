#pragma once

#include <iow/ip/tcp/connection/options.hpp>
#include <iow/io/stream/options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace connection{
  
struct options_json
{
  JSON_NAME(keep_alive)
  typedef options::data_type data_type;
  
  typedef ::wjson::object<
    options,
    ::wjson::member_list<
        ::wjson::base< ::iow::io::stream::options_json<data_type> >,
        ::wjson::member< n_keep_alive, options, bool, &options::keep_alive>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}}}

