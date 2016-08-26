//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <wjrpc/handler/aspect/target.hpp>
#include <wjrpc/handler/aspect/interface_.hpp>
#include <wjrpc/handler/aspect/peeper.hpp>
#include <wjrpc/handler/aspect/connect.hpp>
#include <wjrpc/handler/aspect/disconnect.hpp>
#include <wjrpc/handler/aspect/context.hpp>
#include <wjrpc/handler/aspect/ad_response_handler.hpp>
#include <wjrpc/handler/aspect/ad_perform_send.hpp>

#include <wjrpc/handler/aspect/ad_invoke.hpp>
#include <wjrpc/handler/aspect/ad_invoke_error.hpp>
#include <wjrpc/handler/aspect/ad_initialize.hpp>
#include <wjrpc/handler/handler_base_options.hpp>

#include <wjrpc/method/aspect/request_serializer.hpp>
#include <wjrpc/method/aspect/notify_serializer.hpp>
#include <wjrpc/method/aspect/process_response.hpp>
#include <wjrpc/method/aspect/send_error.hpp>
#include <wjrpc/method/aspect/send_result.hpp>
#include <fas/aop.hpp>

namespace wjrpc{
  
struct aspect_handler: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  peeper<fas::empty_type>, 
  context<fas::empty_type>,
  fas::stub<_connect_>, 
  fas::stub<_disconnect_>,
  request_serializer<80>,
  notify_serializer<80>,
  send_error<80>,
  send_result<80>,
  process_response,
  fas::advice<_response_handler_, ad_response_handler>,
  fas::type<_handler_types_, handler_types>,
  fas::type<_mutex_type_, std::recursive_mutex>,
  fas::advice<_perform_send_, ad_perform_send>,
  //fas::advice<_send_request_, ad_send_request>,
  //fas::advice<_send_notify_, ad_send_notify>,
  fas::advice<_invoke_, ad_invoke>,
  fas::advice<_invoke_error_, ad_invoke_error>,
  fas::advice< _initialize_, ad_initialize>,
  fas::type< _options_type_, handler_base_options>
>{};


} // iow


