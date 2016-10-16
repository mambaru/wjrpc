#include "calc/calc1.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"
#include "calc/api/multiplies_json.hpp"
#include "calc/api/divides_json.hpp"

#include <wjrpc/engine.hpp>
#include <wjrpc/handler.hpp>
#include <wjrpc/method.hpp>

#include <iostream>
#include <functional>

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

namespace gateway
{
  JSONRPC_TAG(plus)
  JSONRPC_TAG(minus)
  JSONRPC_TAG(multiplies)
  JSONRPC_TAG(divides)

  struct method_list: wjrpc::method_list
  <
    wjrpc::call_method<_plus_, request::plus_json, response::plus_json>,
    wjrpc::call_method<_minus_, request::minus_json, response::minus_json>,
    wjrpc::call_method<_multiplies_, request::multiplies_json, response::multiplies_json>,
    wjrpc::call_method<_divides_, request::divides_json, response::divides_json>
  >
  {};

  class handler
    : public ::wjrpc::handler<method_list>
    , public icalc
  {
  public:
    virtual void plus( request::plus::ptr req, response::plus::callback cb)  override
    {
      this->template call<_plus_>( std::move(req), cb, nullptr );
    }

    virtual void minus( request::minus::ptr req, response::minus::callback cb) override
    {
      this->template call<_minus_>( std::move(req), cb, nullptr );
    }

    virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) override
    {
      this->template call<_multiplies_>( std::move(req), cb, nullptr );
    }

    virtual void divides( request::divides::ptr req, response::divides::callback cb) override
    {
      this->template call<_divides_>( std::move(req), cb, nullptr );
    }
  };

  typedef wjrpc::engine<handler> engine_type;
}

int main()
{
  auto dom = std::make_shared<calc1>();
  auto srv = std::make_shared<service::engine_type>();
  auto cli = std::make_shared<gateway::engine_type>();
  
  service::engine_type::options_type srv_opt;
  srv_opt.target = dom;
  srv->start(srv_opt, 1);

  gateway::engine_type::options_type cli_opt;
  cli->start(cli_opt, 2);

  cli->reg_io(22, [srv]( wjrpc::data_ptr d, wjrpc::io_id_t io_id, wjrpc::output_handler_t handler)
  {
    srv->perform_io(std::move(d), io_id, handler);
  });
  
  auto clc = cli->find(22);
  
  auto plus = std::make_unique<request::plus>();
  plus->first = 10;
  plus->second = 20;
  clc->plus( std::move(plus), [](response::plus::ptr res)
  {
    std::cout << "10+20=" << res->value << std::endl;; 
  });

  auto minus = std::make_unique<request::minus>();
  minus->first = 30;
  minus->second = 3;
  clc->minus( std::move(minus), [](response::minus::ptr res)
  {
    std::cout << "30-3=" << res->value << std::endl;; 
  });

}


