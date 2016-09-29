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

int main()
{
  std::list<std::string> req_list = 
  {
    "{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}"
  };

  for ( auto& sreq : req_list )
  {
    std::cout << "-----------------------" << std::endl;
    std::cout << sreq << std::endl;
    wjrpc::incoming_holder inholder( std::move(sreq) );
    inholder.parse(nullptr);

    // Есть имя метода и идентификатор вызова
    if ( inholder.method() == "plus" )
    {
      auto params = inholder.get_params<request::plus_json>(nullptr);
      wjrpc::outgoing_result<response::plus> res;
      res.result = std::make_unique<response::plus>();
      res.result->value = params->first + params->second;
      auto raw_id = inholder.raw_id();
      res.id = std::make_unique<wjrpc::data_type>( raw_id.first, raw_id.second );
      typedef wjrpc::outgoing_result_json<response::plus_json> result_json;
      result_json::serializer()( res, std::ostreambuf_iterator<char>(std::cout) );
      std::cout << std::endl;
    }
    /* else if ( inholder.method() == "minus" ) { ... } */
    /* else if ( inholder.method() == "multiplies" ) { ... } */
    /* else if ( inholder.method() == "divides" ) { ... } */
  }
}


