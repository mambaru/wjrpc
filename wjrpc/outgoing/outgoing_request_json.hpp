//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/outgoing/outgoing_request.hpp>
#include <wjrpc/outgoing/version_member.hpp>
#include <wjson/name.hpp>
#include <wjson/json.hpp>

namespace wjrpc{

template<typename T>
struct outgoing_request_json
{
  typedef typename T::target target;
  typedef outgoing_request<target> request_type;
  typedef typename request_type::version_type version_type;
  typedef wjson::pointer<std::unique_ptr<target>, T> params_json;
  
  JSON_NAME(method)
  JSON_NAME(params)
  JSON_NAME(id)

  typedef ::wjson::object<
    request_type,
    typename fas::type_list_n<
      version_member::type,
      ::wjson::member<n_method, request_type, std::string, &request_type::method>,
      ::wjson::member<n_params, request_type, std::unique_ptr<target>, &request_type::params, params_json >,
      ::wjson::member<n_id, request_type, int, &request_type::id>
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}
