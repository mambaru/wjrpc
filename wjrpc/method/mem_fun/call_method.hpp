//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/call.hpp>
#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/method.hpp>
#include <wjrpc/errors/error_json.hpp>
#include <memory>
#include <functional>

namespace wjrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult,
  typename ...Args
>
struct basic_call_method: basic_method< 
  name<TgName>,
  remote_call<JParams, JResult, error_json>,
  Args...
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename ...Args
>
struct call_method: method< 
  name<TgName>,
  remote_call<JParams, JResult, error_json>,
  Args...
>
{};

} // wjrpc

