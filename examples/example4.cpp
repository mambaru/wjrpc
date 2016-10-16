#include "calc/calc1.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"
#include "calc/api/multiplies_json.hpp"
#include "calc/api/divides_json.hpp"

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

template<typename ResJ>
void send_response(std::shared_ptr<wjrpc::incoming_holder> ph, typename ResJ::target::ptr result, std::string& out)
{
  
  typedef ResJ result_json;
  typedef typename result_json::target result_type;
  wjrpc::outgoing_result<result_type> resp;
  resp.result = std::move(result);
  auto raw_id = ph->raw_id();
  resp.id = std::make_unique<wjrpc::data_type>( raw_id.first, raw_id.second );
  typedef wjrpc::outgoing_result_json<result_json> response_json;
  typename response_json::serializer()( resp, std::back_inserter( out ) );
}

template<
  typename JParams, 
  typename JResult, 
  void (icalc::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  )
>
void invoke(wjrpc::incoming_holder inholder, std::shared_ptr<icalc> calc, std::string& out)
{
  typedef JParams params_json;
  typedef JResult result_json;
  wjson::json_error e;
  auto params = inholder.get_params<params_json>(&e);
  if ( !e )
  {
    std::shared_ptr<wjrpc::incoming_holder> ph = std::make_shared<wjrpc::incoming_holder>( std::move(inholder) );
    (calc.get()->*mem_ptr)( std::move(params), std::bind( send_response<result_json>, ph, std::placeholders::_1, std::ref(out) ) );
  }
  else
  {
    out  = make_error<wjrpc::invalid_params>();
  }
}

int main()
{
  std::vector<std::string> req_list = 
  {
    "{\"method\":\"plus\", \"params\":{ \"first\":2, \"second\":3 }, \"id\" :1 }",
    "{\"method\":\"minus\", \"params\":{ \"first\":5, \"second\":10 }, \"id\" :1 }",
    "{\"method\":\"multiplies\", \"params\":{ \"first\":2, \"second\":2 }, \"id\" :1 }",
    "{\"method\":\"divides\", \"params\":{ \"first\":9, \"second\":3 }, \"id\" :1 }"
  };
  std::vector<std::string> res_list;
 
  auto calc = std::make_shared<calc1>();
  for ( auto& sreq : req_list )
  {
    res_list.push_back( std::string() );
    std::string& out = res_list.back();
    
    wjrpc::incoming_holder inholder( sreq );
    
    wjson::json_error e;
    inholder.parse(&e);
    if ( e )
    {
      out = make_error<wjrpc::parse_error>();
    }
    else if ( inholder.is_request() )
    {
      // Есть имя метода и идентификатор вызова
      if ( inholder.method() == "plus" )
      {
        invoke<request::plus_json, response::plus_json, &icalc::plus>( std::move(inholder), calc, out );
      }
      else if ( inholder.method() == "minus" )
      {
        invoke<request::minus_json, response::minus_json, &icalc::minus>( std::move(inholder), calc, out );
      }
      else if ( inholder.method() == "multiplies" )
      {
        invoke<request::multiplies_json, response::multiplies_json, &icalc::multiplies>( std::move(inholder), calc, out );
      }
      else if ( inholder.method() == "divides" )
      {
        invoke<request::divides_json, response::divides_json, &icalc::divides>( std::move(inholder), calc, out );
      }
      else
      {
        out = make_error<wjrpc::procedure_not_found>();
      }
    }
    else
    {
      out = make_error<wjrpc::invalid_request>();
    }
  }

  for ( size_t i =0; i != res_list.size(); ++i)
  {
    std::cout << req_list[i] << std::endl;
    std::cout << res_list[i] << std::endl;
    std::cout << std::endl;
  }
}


