#include "calc/calc1.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"


#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/outgoing/outgoing_result.hpp>
#include <wjrpc/outgoing/outgoing_result_json.hpp>
#include <wjrpc/outgoing/outgoing_error.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
#include <wjrpc/memory.hpp>
#include <iostream>

template<typename E>
std::string make_error()
{
  typedef wjrpc::outgoing_error<wjrpc::error> parse_error;
  parse_error err;
  err.error = std::make_unique<E>();

  typedef wjrpc::outgoing_error_json<wjrpc::error_json> error_json;
  
  std::string str;
  error_json::serializer()(err, std::back_inserter(str));
  return str;
}

template<typename Req, typename Res>
void invoke(
  std::shared_ptr<icalc> calc, 
  void (icalc:: *method)(Req, typename Req::callback), 
  typename Req::ptr req,
  typename Res::callback cb
)
{
  calc.get()->*method( std::move(req), std::move(cb));
}

template<typename ReqJ, typename ResJ>
std::string perform(wjrpc::incoming_holder /*inholder*/)
{
  /*
  typedef ReqJ params_json;
  typedef typename params_json::target params_type;
  typedef ResJ result_json;
  typedef typename result_json::target result_type;

  wjson::json_error e;
  auto params = inholder.get_params<params_json>(&e);
  if ( !e )
  {
   */ 
    /*
    wjrpc::outgoing_result<params_type> res;
    res.result = std::make_unique<params_type>();
    
    
    res.result->value = params->first + params->second;
    auto raw_id = inholder.raw_id();
    res.id = std::make_unique<wjrpc::data_type>( raw_id.first, raw_id.second );
    
          typedef wjrpc::outgoing_result_json<response::plus_json> result_json;
          result_json::serializer()( res, std::ostreambuf_iterator<char>(std::cout) );
          std::cout << std::endl;
        }
        else
        {
          std::cerr << "CERR: Неверный параметр для '" << inholder.method() << "': " << inholder.params_error_message(e) << std::endl;
          std::cout << make_error<wjrpc::invalid_params>() << std::endl;
        }
  */
  //}
  return "";
}


template<typename ResJ>
void send_responce(std::shared_ptr<wjrpc::incoming_holder> ph, typename ResJ::target::ptr result)
{
  typedef ResJ result_json;
  typedef typename result_json::target result_type;
  wjrpc::outgoing_result<result_type> resp;
  resp.result = std::move(result);
  auto raw_id = ph->raw_id();
  resp.id = std::make_unique<wjrpc::data_type>( raw_id.first, raw_id.second );
  auto d = ph->detach();
  d->clear();
  typedef wjrpc::outgoing_result_json<result_json> response_json;
  typename response_json::serializer()( resp, std::back_inserter( *d ) );
  std::cout << std::string(d->begin(), d->end()) << std::endl;
}

template<
  typename JParams, 
  typename JResult, 
  void (icalc::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  )
>
void invoke(wjrpc::incoming_holder inholder, std::shared_ptr<icalc> calc)
{
  typedef JParams params_json;
  typedef JResult result_json;
  wjson::json_error e;
  auto params = inholder.get_params<params_json>(&e);
  if ( !e )
  {
    std::shared_ptr<wjrpc::incoming_holder> ph = std::make_shared<wjrpc::incoming_holder>( std::move(inholder) );
    (calc.get()->*mem_ptr)( std::move(params), std::bind( send_responce<result_json>, ph, std::placeholders::_1) );
  }
  else
  {
    std::cerr << "CERR: Неверный параметр для '" << inholder.method() << "': " << inholder.params_error_message(e) << std::endl;
    std::cout << make_error<wjrpc::invalid_params>() << std::endl;
  }
}

int main()
{
  auto calc = std::make_shared<calc1>();
  
  std::list<std::string> req_list = 
  {
    "{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}"
  };
  
  for ( auto& sreq : req_list )
  {
    std::cout << "-----------------------" << std::endl;
    std::cout << sreq << std::endl;
    wjrpc::incoming_holder inholder( std::move(sreq) );
    
    wjson::json_error e;
    inholder.parse(&e);
    if ( e )
    {
      std::cerr << "CERR: Ошибка JSON-RPC: " << inholder.error_message(e) << std::endl;
      std::cout << make_error<wjrpc::parse_error>() << std::endl;
    }
    else if ( inholder.is_request() )
    {
      // Есть имя метода и идентификатор вызова
      if ( inholder.method() == "plus" )
      {
        // Ручная обработка
        auto params = inholder.get_params<request::plus_json>(&e);
        if ( !e )
        {
          std::shared_ptr<wjrpc::incoming_holder> ph = std::make_shared<wjrpc::incoming_holder>( std::move(inholder) );
          calc->plus( std::move(params), std::bind( send_responce<response::plus_json>, ph, std::placeholders::_1) );
        }
        else
        {
          std::cerr << "CERR: Неверный параметр для '" << inholder.method() << "': " << inholder.params_error_message(e) << std::endl;
          std::cout << make_error<wjrpc::invalid_params>() << std::endl;
        }
      }
      else
      {
        std::cerr << "CERR: Ошибка: метод '" << inholder.method() <<"' не поддерживается " << std::endl;
        std::cout << make_error<wjrpc::procedure_not_found>() << std::endl;
      }
    }
    if ( inholder.method() == "minus" )
    {
      // Есть имя метода и идентификатор вызова
      invoke<request::minus_json, response::minus_json, &icalc::minus>( std::move(inholder), calc );
    }
    else
    {
      std::cerr << "CERR: Ошибка: это не JSON-RPC запрос" << std::endl;
      std::cout << make_error<wjrpc::invalid_request>() << std::endl;
    }
  }
}


