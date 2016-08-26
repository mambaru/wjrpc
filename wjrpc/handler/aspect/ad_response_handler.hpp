//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wjrpc{
  
struct ad_response_handler
{
  template<
    typename Tg,
    typename Result,
    typename Error
  >
  static inline void process(
    Result res,
    Error err,
    std::function<void(Result)> result_callback,
    std::function<void(Error)> error_callback
  )
  {
    if ( res!=nullptr )
    {
      if (result_callback!=nullptr)
      {
        result_callback( std::move(res) );
      }
    }
    else if ( err!=nullptr )
    {
      if (error_callback!=nullptr)
      {
        error_callback( std::move(err) );
      }
      else if ( result_callback!=nullptr )
      {
        result_callback( nullptr );
      }
    }
    else if (result_callback!=nullptr)
    {
      result_callback(nullptr);
    }
  }
};

} // iow


