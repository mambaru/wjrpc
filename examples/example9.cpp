#include "calc/icalc.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"
#include "calc/api/multiplies_json.hpp"
#include "calc/api/divides_json.hpp"

#include <wjrpc/engine.hpp>
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
  wjrpc::call_method<_plus_, request::plus_json, response::plus_json>,
  wjrpc::call_method<_minus_, request::minus_json, response::minus_json>,
  wjrpc::call_method<_multiplies_, request::multiplies_json, response::multiplies_json>,
  wjrpc::call_method<_divides_, request::divides_json, response::divides_json, icalc>
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

int main()
{
  std::vector<std::string> req_list;
  std::vector<std::string> res_list = 
  {
    "{\"result\":{ \"value\":30 }, \"id\" :1 }",
    "{\"result\":{ \"value\":27 }, \"id\" :2 }"
  };

  typedef wjrpc::engine<handler> engine_type;
  auto e = std::make_shared<engine_type>();
  engine_type::options_type opt;
  e->start(opt, 11);
  
  wjrpc::output_handler_t result_handler;
  e->reg_io(22, [&req_list, &result_handler]( wjrpc::data_ptr d, wjrpc::io_id_t io_id, wjrpc::output_handler_t handler)
  {
    std::cout << "io_id=" << io_id << std::endl;
    if ( !result_handler ) result_handler = handler;
    req_list.push_back( std::string(d->begin(), d->end() ) );
  });
  
  auto h = e->find(22);
  
  auto plus = std::make_unique<request::plus>();
  plus->first = 10;
  plus->second = 20;
  h->plus( std::move(plus), [](response::plus::ptr res)
  {
    std::cout << "10+20=" << res->value << std::endl;; 
  });

  auto minus = std::make_unique<request::minus>();
  minus->first = 30;
  minus->second = 3;
  h->minus( std::move(minus), [](response::minus::ptr res)
  {
    std::cout << "30-3=" << res->value << std::endl;; 
  });

  for ( size_t i =0; i != req_list.size(); ++i)
  {
    std::cout << req_list[i] << std::endl;
    std::cout << res_list[i] << std::endl;
    result_handler( std::make_unique<wjrpc::data_type>(res_list[i].begin(), res_list[i].end() ) );
  }
}


