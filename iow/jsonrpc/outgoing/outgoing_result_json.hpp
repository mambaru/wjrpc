#pragma once

#include <iow/jsonrpc/outgoing/outgoing_result.hpp>
#include <iow/jsonrpc/outgoing/version_member.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_result_json
{
  typedef typename T::target target;
  typedef outgoing_result<target> result_type;
  typedef typename result_type::version_type version_type;

  typedef ::wjson::pointer<std::unique_ptr<target>, T> result_json;
  typedef ::wjson::pointer<data_ptr, ::wjson::raw_value< data_type> > id_json;
  
  JSON_NAME(id)
  JSON_NAME(result)
  
  typedef ::wjson::object<
    result_type,
    typename fas::type_list_n<
      version_member::type,
      ::wjson::member<n_result, result_type, std::unique_ptr<target>,  &result_type::result, result_json >,
      ::wjson::member<n_id,     result_type, data_ptr, &result_type::id,   id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}

