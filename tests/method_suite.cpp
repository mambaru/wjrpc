#include <iostream>
#include <wjrpc/method.hpp>
#include <wjrpc/noparams_json.hpp>
#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/memory.hpp>
#include <wjrpc/handler.hpp>
#include <fas/testing.hpp>
#include "req.hpp"

namespace request{

template<typename T>
struct base 
{
  int first=0; 
  int second=0;
  typedef std::unique_ptr<T> ptr;
};
struct plus: base<plus> {};
struct munus: base<munus> {};

template<typename T>
struct T_json
{
  FAS_NAME(first)
  FAS_NAME(second)

  typedef ::wjson::object<
    T,
    ::wjson::member_list<
      ::wjson::member<n_first, base<T>,  int,  &base<T>::first>,
      ::wjson::member<n_second, base<T>, int,  &base<T>::second>
    >
  > type;
  typedef typename type::serializer serializer;
  typedef typename type::target target;
};

struct plus_json: T_json<plus> {};
struct munus_json: T_json<munus> {};

}

namespace response{

template<typename T>
struct base 
{
  int value=0;
  typedef std::unique_ptr<T> ptr;
  typedef std::function<void(ptr)> callback;
  typedef std::function<void(ptr, ::wjrpc::error::ptr)> callback2;
};

struct plus: base<plus> {};
struct munus: base<munus> {};

template<typename T>
struct T_json
{
  FAS_NAME(value)

  typedef ::wjson::object<
    T,
    ::wjson::member_list<
      ::wjson::member<n_value, base<T>, int, &base<T>::value>
    >
  > type;
  typedef typename type::serializer serializer;
  typedef typename type::target target;
};

struct plus_json: T_json<plus> {};
struct munus_json: T_json<munus> {};

}

struct plus_handler
{
  template<typename T>
  void operator() ( T&, request::plus::ptr req, response::plus::callback2 cb)
  {
    
    /*
    using namespace fas::testing;
    t << is_true<assert>( req!=nullptr ) << FAS_FL;
    t << is_true<assert>( cb!=nullptr ) << FAS_FL;
    t << stop;
    */
    auto res = std::make_unique<response::plus>();
    res->value = req->first + req->second;
    cb( std::move(res), nullptr );
  }
};

JSONRPC_TAG(plus)
JSONRPC_TAG(minus)

struct method_plus:
  ::wjrpc::basic_method<
    ::wjrpc::name<_plus_>,
    ::wjrpc::invoke< request::plus_json, response::plus_json, plus_handler >,
    ::wjrpc::log_aspect, 
    ::wjrpc::send_result<80>
>
{};


UNIT(method1, "")
{
  using namespace fas::testing;
  //using namespace ::wjrpc;

  ::wjrpc::handler< ::wjrpc::method_list<> > h;
  std::string sreq="{\"method\":\"plus\", \"id\":1, \"params\":{\"first\":1, \"first\":2}}";
  method_plus m;
  ::wjrpc::incoming_holder holder( std::make_unique<::wjrpc::data_type>( sreq.begin(), sreq.end() ) );
  ::wjson::json_error e;
  holder.parse(&e);
  t << is_false<assert>( e ) << FAS_FL;
  t << stop;
  m( h, std::move(holder), [&t]( ::wjrpc::outgoing_holder res )
  {
    auto d = res.detach();
    t << message("TRACE:") << std::string(d->begin(), d->end() );
  } );
  
  t << nothing;
}

BEGIN_SUITE(method_suite, "")
  ADD_UNIT(method1)
END_SUITE(method_suite)