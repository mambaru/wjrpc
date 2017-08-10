//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/errors.hpp>
#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/memory.hpp>

namespace wjrpc{

struct invoke_stub
{
  typedef ::fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef invoke_stub advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  template<typename T, typename TT, typename OutgoingHandler>
  void operator()(
    T&, 
    TT&, 
    incoming_holder holder,
    OutgoingHandler outgoing_handler
  ) const
  {
    TT::template send_error<T, error_json>( 
      std::move(holder), 
      std::make_unique<method_not_impl>(), 
      std::move(outgoing_handler) 
    );
  }
};

} // wjrpc


