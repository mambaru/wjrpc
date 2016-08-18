#pragma once

#include <wjrpc/method/aspect/call.hpp>
#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/method.hpp>
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
  call<JParams, JResult>,
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
  call<JParams, JResult>,
  Args...
>
{};

} // iow

