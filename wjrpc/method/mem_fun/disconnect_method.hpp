//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/handler/aspect/disconnect.hpp>
#include <wjrpc/method/mem_fun/mem_fun_disconnect.hpp>
#include <memory>


namespace wjrpc{

template<
  typename Target, 
  void (Target::*mem_ptr)( io_id_t ) 
>
struct disconnect_method: disconnect< mem_fun_disconnect<Target, mem_ptr> > {};

} // wjrpc

