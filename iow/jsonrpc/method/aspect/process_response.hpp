#pragma once

#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <iow/logger/logger.hpp>
#include <wjson/error.hpp>
#include <wjson/strerror.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
struct process_response
  : fas::type<_process_response_, process_response>
{
  template<typename T, typename JResult, typename JError>
  static inline void process(
    incoming_holder holder, 
    std::function< void (
      std::unique_ptr<typename JResult::target>, 
      std::unique_ptr<typename JError::target>
    )> callback
  ) 
  {
    // получатель
    if ( holder.is_response() )
    {
      ::wjson::json_error e;
      auto pres = holder.template get_result<JResult>(&e);
      if ( !e )
      {
        callback( std::move(pres), nullptr);
      }
      else
      {
        //!! WJRPC_LOG_ERROR("iow::jsonrpc::process_response (result): json error: " << ::wjson::strerror::what( e ) << std::endl << holder.result_error_message(e) );
        callback( nullptr, nullptr);
      }
    }
    else if ( holder.is_error() )
    {
      ::wjson::json_error e;
      auto perr = holder.template get_error<JError>(&e);
      if ( !e )
      {
        callback( nullptr, std::move(perr) );
      }
      else
      {
        //!! WJRPC_LOG_ERROR("iow::jsonrpc::process_response (error): json error: " << ::wjson::strerror::what(e) << std::endl << holder.result_error_message(e) );
        callback( nullptr, nullptr);
      }
    }
    else
    {
      callback( nullptr, nullptr);
    }
  }
};

struct process_response_proxy
  : fas::type<_process_response_, process_response_proxy>
{
  template<typename T, typename JResult, typename JError>
  static inline void process(
    incoming_holder holder, 
    std::function< void (
      std::unique_ptr<typename JResult::target>, 
      std::unique_ptr<typename JError::target>
    )> callback
  ) 
  {
    return T::aspect::template advice_cast<_process_response_>::type
            ::template process<T, JResult, JError>( 
                std::move(holder), 
                std::move(callback) 
              );
  }
};


}} // iow


