#pragma once

#include <wjrpc/handler/aspect/connect.hpp>
#include <wjrpc/method/mem_fun/mem_fun_connect.hpp>
#include <memory>

namespace wjrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( io_id_t, std::weak_ptr<Interface> ) 
>
struct connect_method: connect< mem_fun_connect<Interface, Target, mem_ptr> > {};

} // iow

