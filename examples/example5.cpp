#include "calc/calc1.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"
#include "calc/api/multiplies_json.hpp"
#include "calc/api/divides_json.hpp"

#include <wjrpc/handler.hpp>
#include <wjrpc/method.hpp>

#include <iostream>
#include <functional>


JSONRPC_TAG(plus)
JSONRPC_TAG(minus)
JSONRPC_TAG(multiplies)
JSONRPC_TAG(divides)

struct method_list: wjrpc::method_list
<
  wjrpc::target<icalc>,
  wjrpc::invoke_method<_plus_, request::plus_json, response::plus_json, icalc, &icalc::plus>,
  wjrpc::invoke_method<_minus_, request::minus_json, response::minus_json, icalc, &icalc::minus>,
  wjrpc::invoke_method<_multiplies_, request::multiplies_json, response::multiplies_json, icalc, &icalc::multiplies>,
  wjrpc::invoke_method<_divides_, request::divides_json, response::divides_json, icalc, &icalc::divides>
>{};

class handler: public ::wjrpc::handler<method_list> {};

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
  handler h;
  handler::options_type opt;
  opt.target = calc;
  h.start(opt, 1);

  for ( auto& sreq : req_list )
  {
    h.perform_string( sreq, [&res_list](std::string out) { res_list.push_back(out);} );
  }

  for ( size_t i =0; i != res_list.size(); ++i)
  {
    std::cout << req_list[i] << std::endl;
    std::cout << res_list[i] << std::endl;
    std::cout << std::endl;
  }
}
