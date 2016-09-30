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
  std::vector<std::string> req_list = 
  {
    "{\"method\":\"plus\", \"params\":{ \"first\":2, \"second\":3 }, \"id\" :1 }",
    "{\"method\":\"minus\", \"params\":{ \"first\":5, \"second\":10 }, \"id\" :1 }",
    "{\"method\":\"multiplies\", \"params\":{ \"first\":2, \"second\":2 }, \"id\" :1 }",
    "{\"method\":\"divides\", \"params\":{ \"first\":9, \"second\":3 }, \"id\" :1 }"
  };
  std::vector<std::string> res_list;

  for ( auto& sreq : req_list )
  {
    wjrpc::incoming_holder inholder( sreq );
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
      res_list.push_back(std::string());
      result_json::serializer()( res, std::back_inserter(res_list.back()) );
    }
    /* else if ( inholder.method() == "minus" ) { ... } */
    /* else if ( inholder.method() == "multiplies" ) { ... } */
    /* else if ( inholder.method() == "divides" ) { ... } */
  }

  for ( size_t i =0; i != res_list.size(); ++i)
  {
    std::cout << req_list[i] << std::endl;
    std::cout << res_list[i] << std::endl;
    std::cout << std::endl;
  }
}


