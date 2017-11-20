#include <wjrpc/handler.hpp>
#include <wjrpc/engine.hpp>
#include <wjrpc/method.hpp>
#include <fas/testing.hpp>

#include <algorithm>
#include <memory>

namespace {


namespace request{

/// -------------------
template<typename T>
struct base 
{
  int first=0; 
  int second=0;
  typedef std::unique_ptr<T> ptr;
};

struct plus: base<plus> {};
struct munus: base<munus> {};

/// -------------------
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
};

struct plus: base<plus> {};
struct munus: base<munus> {};

/// -------------------

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

struct icalc
{
  virtual ~icalc() {}
  virtual void plus( request::plus::ptr req, response::plus::callback cb) = 0;
  virtual void minus( request::plus::ptr req, response::plus::callback cb) = 0;
};

class calc
  : public icalc
{
public:
  virtual void plus( request::plus::ptr req, response::plus::callback cb)
  {
    if ( cb == nullptr )
      return;

    if ( req == nullptr )
      return cb(nullptr);

    auto res = std::make_unique<response::plus>();
    res->value = req->first + req->second;
    cb( std::make_unique<response::plus>() );
  }

  virtual void minus( request::plus::ptr , response::plus::callback cb)
  {
    if ( cb == nullptr )
      return;
    return cb(nullptr);
  }
};

JSONRPC_TAG(plus)
JSONRPC_TAG(minus)

struct method_list: wjrpc::method_list
<
  wjrpc::target<icalc>,
  wjrpc::invoke_method<_plus_, request::plus_json, response::plus_json, icalc, &icalc::plus>
>{};

struct handler: ::wjrpc::handler<method_list> {};


UNIT(engine1, "")
{
  using namespace ::fas::testing;
  using namespace ::wjrpc;
  typedef ::wjrpc::engine< handler > engine_type;
  //typedef std::shared_ptr<engine_type> engine_ptr;
  engine_type::options_type opt;
  auto pcalc = std::make_shared<calc>();
  opt.target = pcalc;
  /*opt.sender_handler = [](const char*, notify_serializer_t, request_serializer_t, result_handler_t )
  {
    
  };*/
  auto e = std::make_shared<engine_type>();
  e->start(opt, 1);
  std::string sreq = "{\"method\":\"test1\",\"params\":{\"first\":2, \"first\":3},\"id\":1}";
  e->perform_io( std::make_unique<data_type>(sreq.begin(), sreq.end()), 1, [&t](data_ptr d) 
  {
    using namespace ::fas::testing;
    t << message("responce: ") << std::string( d->begin(), d->end() );
  });
  
  t << nothing;
}

}

BEGIN_SUITE(engine_suite, "")
  ADD_UNIT(engine1)
END_SUITE(engine_suite)
