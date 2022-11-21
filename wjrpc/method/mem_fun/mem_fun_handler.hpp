//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/mem_fun/mem_fun_helper.hpp>

namespace wjrpc{

template<
  typename Params, 
  typename Result, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<Params>, 
    std::function< void(std::unique_ptr<Result>) > 
  ) 
>
struct mem_fun_handler
{
  typedef std::unique_ptr<Params> request_ptr;
  typedef std::unique_ptr<Result> responce_ptr;
  typedef std::unique_ptr<error> error_ptr;
  typedef std::function< void(responce_ptr, error_ptr) > jsonrpc_callback;

  /*
  template<typename T>
  void operator()(T& t, request_ptr req) const
  {
    if ( auto i = t.target() )
    {
      (i.get()->*mem_ptr)(std::move(req), nullptr );
    }
  }*/

  template<typename T>
  void operator()(T& t, request_ptr req, jsonrpc_callback cb) const
  {
    if ( auto i = t.target() )
    {
      (i.get()->*mem_ptr)( 
        std::move(req), 
        mem_fun_make_callback( t, std::move(cb)) 
      );
    }
    else 
    {
      mem_fun_service_unavailable( std::move(cb) );
    }
  }
};

} // wjrpc


