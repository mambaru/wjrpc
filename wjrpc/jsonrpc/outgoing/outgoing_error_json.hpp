#pragma once

#include <iow/jsonrpc/outgoing/outgoing_error.hpp>
#include <iow/jsonrpc/outgoing/version_member.hpp>
#include <iow/jsonrpc/basic_types.hpp>
#include <wjson/json.hpp>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_error_json
{
  typedef ::iow::jsonrpc::data_type data_type;
  typedef ::iow::jsonrpc::data_ptr  data_ptr;
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef ::wjson::pointer<std::unique_ptr<target>, T> error_json;
  typedef ::wjson::pointer<
              std::unique_ptr<data_type>, 
              ::wjson::raw_value< data_type> 
          > id_json;
  
  JSON_NAME(error)
  JSON_NAME(id)
  
  typedef ::wjson::object<
    error_type,
    ::wjson::member_list<
      version_member::type,
      ::wjson::member<n_error, error_type, std::unique_ptr<target>, &error_type::error, error_json >,
      ::wjson::member<n_id,    error_type, data_ptr, &error_type::id,  id_json >
    >
  > type;

  typedef typename type::serializer serializer;
};

}}

