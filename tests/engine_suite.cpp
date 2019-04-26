#include <wjrpc/handler.hpp>
#include <wjrpc/engine.hpp>
#include <wjrpc/method.hpp>
#include <wjson/_json.hpp>
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
struct minus: base<minus> 
{
  static minus create_schema()
  {
    minus m;
    m.first = 11;
    m.second = 22;
    return m;
  }
};

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
struct minus_json: T_json<minus> {};

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
struct minus: base<minus> {};

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
struct minus_json: T_json<minus> {};

}

struct icalc
{
  virtual ~icalc() {}
  virtual void plus( request::plus::ptr req, response::plus::callback cb) = 0;
  virtual void minus( request::minus::ptr req, response::minus::callback cb) = 0;
};

class calc
  : public icalc
{
public:
  virtual void plus( request::plus::ptr req, response::plus::callback cb) override
  {
    if ( cb == nullptr )
      return;

    if ( req == nullptr )
      return cb(nullptr);

    auto res = std::make_unique<response::plus>();
    res->value = req->first + req->second;
    cb( std::move(res) );
  }

  virtual void minus( request::minus::ptr , response::minus::callback cb) override
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
  wjrpc::invoke_method<_plus_, request::plus_json, response::plus_json, icalc, &icalc::plus>,
  wjrpc::invoke_method<_minus_, request::minus_json, response::minus_json, icalc, &icalc::minus>
>{};

struct handler: ::wjrpc::handler<method_list> {};

UNIT(engine1, "")
{
  using namespace ::fas::testing;
  using namespace ::wjrpc;
  using namespace wjson::literals;
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
  std::string sreq = "{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}";
  e->perform_io( std::make_unique<data_type>(sreq.begin(), sreq.end()), 1, [&t](data_ptr d) 
  {
    using namespace ::fas::testing;
    std::string ress( d->begin(), d->end() );
    t << message("responce: ") << ress;
    t << equal<expect, std::string>( ress, "{'jsonrpc':'2.0','result':{'value':5},'id':1}"_json);
  });
  
  auto schl = e->create_schema();
  std::string sch_json;
  decltype(schl)::value_type::json::list::serializer()(schl, std::back_inserter(sch_json) );
  t << equal<expect, std::string>( sch_json, "[{'name':'plus','params':{'first':0,'second':0},'result':{'value':0},'error':{'code':0,'message':'No error'}},{'name':'minus','params':{'first':11,'second':22},'result':{'value':0},'error':{'code':0,'message':'No error'}}]"_json);
  t << nothing;
}

}

BEGIN_SUITE(engine_suite, "")
  ADD_UNIT(engine1)
END_SUITE(engine_suite)
