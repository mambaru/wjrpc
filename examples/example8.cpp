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
  //wjrpc::interface_<icalc>,
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

int main()
{
  wjrpc::call_id_t call_id = 100;
  std::vector<std::string> req_list;
  auto h = std::make_shared<handler>();
  handler::options_type opt;
  opt.sender_handler = [&req_list, &call_id](
    const char* name, 
    wjrpc::handler_types::notify_serializer_t /*ns1*/, 
    wjrpc::handler_types::request_serializer_t rs1, 
    wjrpc::handler_types::result_handler_t /*rh1*/)
  {
    auto d = rs1(name, call_id++ );
    std::string sreq(d->begin(), d->end() );
    req_list.push_back(sreq );
  };
  h->start(opt, 1);
  
  auto plus = std::make_unique<request::plus>();
  plus->first = 10;
  plus->second = 20;
  h->plus( std::move(plus), [](response::plus::ptr) noexcept{});

  auto minus = std::make_unique<request::minus>();
  minus->first = 30;
  minus->second = 3;
  h->minus( std::move(minus), [](response::minus::ptr) noexcept{});

  for ( size_t i =0; i != req_list.size(); ++i)
  {
    std::cout << req_list[i] << std::endl;
  }
}



