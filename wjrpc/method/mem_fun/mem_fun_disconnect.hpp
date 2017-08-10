//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/mem_fun/mem_fun_helper.hpp>

namespace wjrpc{

template<
  typename Target, 
  void (Target::*mem_ptr)( io_id_t ) 
>
struct mem_fun_disconnect
{
  template<typename T>
  void operator()(T& t, io_id_t id) const
  {
    if (auto trg = t.peeper() )
    {
      (trg.get()->*mem_ptr)( id );
    }
    else
    {
      // abort();
    }
  }
};

} // wjrpc


