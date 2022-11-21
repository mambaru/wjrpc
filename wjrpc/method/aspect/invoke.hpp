//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/errors.hpp>
#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/logger.hpp>
#include <wjrpc/memory.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace wjrpc{

template<typename JParams, typename JResult, typename Handler, typename JError = error_json>
struct invoke: Handler
{
  typedef invoke<JParams, JResult, Handler, JError> self;
  typedef self advice_class;
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  typedef JParams params_json_t;
  typedef JResult result_json_t;
  typedef JError  error_json_t;

  typedef typename params_json_t::target  params_type;
  typedef typename result_json_t::target  result_type;
  typedef typename error_json_t::target   error_type;

  typedef typename std::unique_ptr<params_type> params_ptr;
  typedef typename std::unique_ptr<result_type> result_ptr;
  typedef typename std::unique_ptr<error_type>  error_ptr;

  template<typename T, typename TT, typename OutgoingHandler>
  void operator()(
    T& t, 
    TT& ,
    incoming_holder holder, 
    OutgoingHandler outgoing_handler
  ) const
  {
    ::wjson::json_error e;
    params_ptr req = holder.template get_params<params_json_t>(&e);

    if ( e )
    {
      WJRPC_LOG_ERROR("jsonrpc::invoke Invalid Params: " << holder.params_error_message(e) );

      if ( holder.has_id() && outgoing_handler!=nullptr )
      {
        TT::template send_error<T, error_json_t>( 
          std::move(holder), 
          std::make_unique<invalid_params>(), 
          std::move(outgoing_handler) 
        );
      }
      return;
    }

    /*if ( holder.is_notify() || outgoing_handler==nullptr )
    {
      Handler::operator()( t, std::move(req) );
    }
    else
    {*/
    using namespace std::placeholders;
    auto ph = std::make_shared<incoming_holder>( std::move(holder) );
    Handler::operator()( t, std::move(req),
      [ph, outgoing_handler]( result_ptr result, error_ptr err )
      {
        if (err == nullptr )
        {
          TT::template send_result<T, result_json_t>(
            std::move(*ph),
            std::move(result),
            std::move(outgoing_handler)
          );
        }
        else
        {
          TT::template send_error<T, error_json_t>(
            std::move(*ph),
            std::move(err),
            std::move(outgoing_handler)
          );
        }
      }
    );
    /*}*/
  }
  
  template <typename OItr>
  static OItr serialize_params(const params_type& params, OItr oitr )
  {
    return typename params_json_t::serializer()(params, oitr);
  }
  
  template <typename OItr>
  static OItr serialize_result(const result_type& result, OItr oitr )
  {
    return typename result_json_t::serializer()(result, oitr);
  }

  template <typename OItr>
  static OItr serialize_error(const error_type& error, OItr oitr )
  {
    return typename error_json_t::serializer()(error, oitr);
  }
  
};


} // wjrpc


