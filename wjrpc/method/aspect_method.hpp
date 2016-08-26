//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/aspect/call.hpp>
#include <wjrpc/method/aspect/invoke.hpp>

#include <wjrpc/method/aspect/invoke_stub.hpp>
#include <wjrpc/method/aspect/send_error.hpp>
#include <wjrpc/method/aspect/send_result.hpp>
#include <wjrpc/method/aspect/request_serializer.hpp>
#include <wjrpc/method/aspect/notify_serializer.hpp>
#include <wjrpc/method/aspect/process_response.hpp>
#include <wjrpc/method/aspect/tags.hpp>

#include <fas/aop.hpp>

namespace wjrpc{
  
struct aspect_method
  : fas::aspect< 
      request_serializer_proxy,
      notify_serializer_proxy,
      process_response_proxy,
      send_error_proxy,
      send_result_proxy,
      invoke_stub
    > 
{
};

} // iow


