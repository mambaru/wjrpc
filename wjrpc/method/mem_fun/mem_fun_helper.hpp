//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/errors.hpp>
#include <wjrpc/memory.hpp>
#include <functional>

namespace wjrpc{

template<typename Result>
inline auto mem_fun_make_callback( std::function< void(std::unique_ptr<Result>, std::unique_ptr< error>) >&& cb)
  -> std::function<void(std::unique_ptr<Result>) >
{
  if (cb==nullptr)
    return nullptr;

  return [cb]( std::unique_ptr<Result> resp)
  {
    if ( resp != nullptr )
    {
      cb( std::move(resp), nullptr);
    }
    else
    {
      cb( nullptr, std::make_unique< error >( bad_gateway() ) );
    }
  };
}

template<typename Result>
inline void mem_fun_service_unavailable( std::function< void(std::unique_ptr<Result>, std::unique_ptr< error>) >&& cb)
{
  if (cb!=nullptr)
  {
    cb( nullptr, std::make_unique< error >( service_unavailable() ) );
  }
}

} // iow


