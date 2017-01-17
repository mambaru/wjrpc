//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/outgoing/outgoing_result_json.hpp>
#include <wjrpc/outgoing/outgoing_result.hpp>
#include <wjrpc/memory.hpp>
#include <fas/aop.hpp>

#include <iostream>

namespace wjrpc{
  
template<size_t ReserveSize>
struct send_result
  : fas::type<_send_result_, send_result<ReserveSize> >
{
  template<typename T, typename JResult, typename OutgoingHandler>
  static inline void send(
    incoming_holder holder, 
    std::unique_ptr<typename JResult::target> result, 
    OutgoingHandler outgoing_handler
  )
  {
    typedef JResult result_json;
    typedef typename JResult::target result_type;
    
    outgoing_result<result_type> result_message;
    result_message.result = std::move(result);
              
    auto id_range = holder.raw_id();
    result_message.id = 
      std::make_unique<typename T::data_type>( 
          id_range.first, 
          id_range.second 
      );
    if ( auto d = holder.detach() )
    {
      d->clear();
      d->reserve(ReserveSize);
      typedef outgoing_result_json<result_json> message_json;
      typename message_json::serializer()(
        result_message, 
        std::inserter( *d, d->end() )
      );
      outgoing_handler( std::move(d) );
    }
    else
      outgoing_handler( nullptr );
  }
};

struct send_result_proxy
  : fas::type<_send_result_, send_result_proxy >
{
  template<typename T, typename JResult, typename OutgoingHandler>
  static inline void send(
    incoming_holder holder, 
    std::unique_ptr<typename JResult::target> res, 
    OutgoingHandler outgoing_handler
  )
  {
    return T::aspect::template advice_cast<_send_result_>::type
            ::template send<T, JResult>( 
                std::move(holder), 
                std::move(res), 
                std::move(outgoing_handler) 
            );
  }
};


} // iow


