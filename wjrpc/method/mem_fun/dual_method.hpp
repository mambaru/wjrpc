//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/call.hpp>
#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/mem_fun/invoke_mem_fun.hpp>
#include <wjrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wjrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ),
  typename ...Args
>
struct basic_dual_method: basic_method< 
  name<TgName>,
  invoke_mem_fun<JParams,JResult,I,mem_ptr>,
  remote_call<JParams, JResult>,
  Args...
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ),
  typename ...Args
>
struct dual_method: method< 
  name<TgName>,
  invoke_mem_fun<JParams, JResult, I, mem_ptr>,
  remote_call<JParams, JResult>,
  Args...
>
{};


} // iow


