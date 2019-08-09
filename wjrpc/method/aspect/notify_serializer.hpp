//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/outgoing/outgoing_notify.hpp>
#include <wjrpc/outgoing/outgoing_notify_json.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wjrpc{

template< size_t ReserveSize = 80 >
struct notify_serializer
  : fas::type<_notify_serializer_, notify_serializer<ReserveSize> >
{
  template<typename T, typename JParams>
  static inline auto
  serialize(
    const char* name,
    std::unique_ptr<typename JParams::target> req
  ) -> typename T::data_ptr
  {
    typedef typename JParams::target params_type;
    outgoing_notify<params_type> notify;
    notify.params = std::move(req);
    notify.method = name;
    auto d = std::make_unique< typename T::data_type>();
    d->reserve(ReserveSize);
    typedef outgoing_notify_json<JParams> send_json;
    typename send_json::serializer()(notify, std::inserter( *d, d->end() ));
    return d;
  }
};

struct notify_serializer_proxy
  : fas::type<_notify_serializer_, notify_serializer_proxy >
{
  template<typename T, typename JParams>
  static inline auto
  serialize(
    const char* name,
    std::unique_ptr<typename JParams::target> req
  ) -> typename T::data_ptr
  {
    return T::aspect::template advice_cast<_notify_serializer_>::type
            ::template serialize<T, JParams>( name, std::move(req) );
  }
};


} // wjrpc


