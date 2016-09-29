#include "calc/api/plus.hpp"
#include "calc/api/plus_json.hpp"

#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/outgoing/outgoing_result.hpp>
#include <wjrpc/outgoing/outgoing_result_json.hpp>
#include <wjrpc/outgoing/outgoing_error.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
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

int main()
{
  std::list<std::string> req_list = 
  {
    "uyi {\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}"
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
        auto params = inholder.get_params<request::plus_json>(&e);
        if ( !e )
        {
          wjrpc::outgoing_result<response::plus> res;
          res.result = std::make_unique<response::plus>();
          res.result->value = params->first + params->second;
          res.id = std::make_unique<wjrpc::data_type>( inholder.raw_id().first, inholder.raw_id().second );
          typedef wjrpc::outgoing_result_json<response::plus_json> result_json;
          result_json::serializer()( res, std::ostreambuf_iterator<char>(std::cout) );
          std::cout << std::endl;
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
    else
    {
      std::cerr << "CERR: Ошибка: это не JSON-RPC запрос" << std::endl;
      std::cout << make_error<wjrpc::invalid_request>() << std::endl;
    }
  }
}


