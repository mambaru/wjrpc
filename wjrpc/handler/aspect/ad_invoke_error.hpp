//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <memory>

namespace wjrpc{
  
struct ad_invoke_error
{
  template<
    typename T,
    typename Holder, 
    typename Error, 
    typename OutgoingHandler
  >
  void operator()(T&, Holder holder, Error e, OutgoingHandler ohandler)
  {
    T::aspect::template advice_cast<_send_error_>::type
      ::template send<T, error_json>( 
          std::move(holder), 
          std::move(e), 
          std::move(ohandler) 
      );
  }
};


} // wjrpc


