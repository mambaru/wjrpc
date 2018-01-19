//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <fas/aop.hpp>
#include <memory>
#include <wjrpc/logger.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>

namespace wjrpc{
 
struct ad_perform_send
{
  template<typename T, typename Params, typename NotifySerializer, typename RequestSerializer, typename ResultHandler >
  void operator()(
    T& t,
    const char* name,
    Params params, 
    NotifySerializer ns1,
    RequestSerializer rs1,
    ResultHandler rh
  ) const
  {
    std::shared_ptr<Params> p = nullptr;

    if (params!=nullptr)
    {
      p = std::make_shared<Params>( std::move(params) );
    }

    t.sender_handler(
      name,
      [p, ns1](const char* nname) -> typename T::data_ptr
      {
        return ns1(nname, std::move(*p));
      },
      [p, rs1](const char* rname, typename T::call_id_t id) 
        -> typename T::data_ptr
      {
        return rs1(rname, std::move(*p), id);
      },
      std::move(rh)
    );
  }
};

} // wjrpc


