//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/mem_fun/mem_fun_helper.hpp>
#include <wjrpc/handler/aspect/tags.hpp>
#include <memory>
#include <type_traits>

namespace wjrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( io_id_t, std::weak_ptr<Interface> ) 
>
struct mem_fun_connect
{
  template<typename T>
  void operator()(T& t, io_id_t id) const
  {
    if (auto trg = t.peeper() )
    {
      typedef typename T::aspect::template advice_cast<_peeper_>::type::element_type elemet_type;
      static_assert( std::is_same<elemet_type, Interface>::value, "add peeper<Interface> to method list");
      (trg.get()->*mem_ptr)( id, t.shared_from_this() );
    }
    else
    {
      // abort();
    }
  }
};

} // iow


