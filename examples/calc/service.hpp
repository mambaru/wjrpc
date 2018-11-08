#include <calc/icalc.hpp>
#include <calc/api/plus_json.hpp>
#include <calc/api/minus_json.hpp>
#include <calc/api/multiplies_json.hpp>
#include <calc/api/divides_json.hpp>

#include <wjrpc/engine.hpp>
#include <wjrpc/handler.hpp>
#include <wjrpc/method.hpp>

namespace service
{
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

  typedef wjrpc::engine<handler> engine_type;
}
