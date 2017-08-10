//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/invoke.hpp>
#include <wjrpc/method/mem_fun/mem_fun_handler2.hpp>

namespace wjrpc{
 
template<
  typename JParams, 
  typename JResult, 
  typename Params2, 
  typename Result2, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::function< void(
      std::unique_ptr<Params2>, 
      std::function< void(std::unique_ptr<Result2>) >  
    ) >
  ),
  typename Itf,
  void (Itf::*mem_ptr2)( 
    std::unique_ptr<Params2>, 
    std::function< void(std::unique_ptr<Result2>) >
  )
>
struct invoke_mem_fun2
  : invoke<
      JParams, 
      JResult, 
      mem_fun_handler2<
        typename JParams::target, 
        typename JResult::target,
        Params2,
        Result2,
        Target, 
        mem_ptr,
        Itf,
        mem_ptr2
      > 
    >
{};



} // wjrpc


