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

namespace gateway
{
  JSONRPC_TAG(plus)
  JSONRPC_TAG(minus)
  JSONRPC_TAG(multiplies)
  JSONRPC_TAG(divides)

  struct method_list: wjrpc::method_list
  <
    wjrpc::interface_<icalc>,
    wjrpc::call_method<_plus_, request::plus_json, response::plus_json>,
    wjrpc::call_method<_minus_, request::minus_json, response::minus_json>,
    wjrpc::call_method<_multiplies_, request::multiplies_json, response::multiplies_json>,
    wjrpc::call_method<_divides_, request::divides_json, response::divides_json, icalc>
  >
  {};

  template<typename Base>
  class calc_interface
    : public Base 
    , public Base::interface_type
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

  typedef wjrpc::engine< ::wjrpc::handler< calc_interface<method_list> > > engine_type;
}

