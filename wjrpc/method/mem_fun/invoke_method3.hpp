#pragma once

#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/mem_fun/invoke_mem_fun3.hpp>
#include <wjrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wjrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t
  ),
  typename ...Args
>
struct basic_invoke_method3: basic_method< 
  name<TgName>,
  invoke_mem_fun3< JParams, JResult, Target, mem_ptr>,
  Args...
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t
  ),
  typename ...Args
>
struct invoke_method3: method< 
  name<TgName>,
  invoke_mem_fun3< JParams, JResult, Target, mem_ptr>,
  Args...
>
{};


} // iow


