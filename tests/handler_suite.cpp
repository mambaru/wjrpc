#include <wjrpc/handler.hpp>
#include <wjrpc/method.hpp>
#include <wjrpc/method/default_schema.hpp>

#include <fas/testing.hpp>
#include "req.hpp"
#include <algorithm>
#include <memory>

struct nomethod:
  ::wjrpc::method_list<>
{
};

typedef wjrpc::handler<nomethod> nohandler;

int test_count = 0;

UNIT(nohandler_unit, "")
{
  using namespace fas::testing;
  using namespace ::wjrpc;
  
  nohandler h;
  for (auto r: bad_request)
  {
    auto req = std::make_unique<data_type>( r[0].begin(), r[0].end() );
    incoming_holder hold( std::move(req) );
    hold.parse(nullptr);
    h.invoke(std::move(hold), [&t, r](outgoing_holder holder)
    {
      data_ptr res = holder.detach();
      t << message("request: ") << r[0];
      t << message("response: ") << std::string(res->begin(), res->end());
      if ( std::string(res->begin(), res->end()) != r[1] )
      {
        t << ::fas::testing::error("should be:") << r[1];
      }
      ++test_count;
    });
  }

  t << equal<expect>(test_count, 6) << FAS_TESTING_FILE_LINE;
  
}


// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////

typedef std::vector<int> test1_params;
typedef wjson::array< std::vector< wjson::value<int> > > test1_json;


struct itest1
{
  virtual ~itest1() {}
  virtual void method1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback) = 0;
  virtual void method2(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback) = 0;
};

class test1: public itest1
{
public:
  
  virtual void method1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback)
  {
    if ( req==nullptr  )
    {
      if (callback!=nullptr) 
        return callback( std::make_unique<test1_params>() );
      return;
    }
    
    std::reverse(req->begin(), req->end());
    
    if ( callback )
      callback(std::move(req));
  }

  virtual void method2(std::unique_ptr<test1_params> , std::function< void(std::unique_ptr<test1_params>) > callback)
  {
    if ( callback )
      callback(nullptr);
  }
};


JSONRPC_TAG(method1)
JSONRPC_TAG(method2)


struct method_list: wjrpc::method_list
<
  wjrpc::target<itest1>,
  wjrpc::invoke_method< _method1_, test1_json,      test1_json,      itest1, &itest1::method1>,
  wjrpc::invoke_method< _method2_, test1_json,      test1_json,      itest1, &itest1::method2>
>
{};

typedef wjrpc::handler<method_list> handler1;


UNIT(handler1_unit, "")
{
  using namespace fas::testing;
  using namespace ::wjrpc;

   std::string unavailable = "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32003,\"message\":\"Service unavailable\"},\"id\":1}";
  
  test_count = 0;
  handler1 h;
  auto r = good_request[0];
  {
    auto req = std::make_unique< data_type>( r[0].begin(), r[0].end() );
    incoming_holder hold( std::move(req) );
    hold.parse(nullptr);
    
    h.invoke(std::move(hold), [&t, unavailable, r](outgoing_holder holder)
    {
      data_ptr res = holder.detach();
      t << message("request: ") << r[0];
      t << message("response: ") << std::string(res->begin(), res->end());
      if ( std::string(res->begin(), res->end()) != unavailable )
      {
        t << ::fas::testing::error("should be:") << unavailable;
      }
      ++test_count;
    });
  }
}

UNIT(gen1, "")
{
  using namespace fas::testing;
  
  typedef method_list::aspect::template advice_cast<_method1_>::type method1_t;
  typedef method1_t::aspect::template advice_cast< wjrpc::_invoke_>::type::params_json_t request1_json;
  
  typedef request1_json::target value_type;
  value_type val;
  val.push_back(1);
  val.push_back(2);
  std::string json;
  request1_json::serializer()(val, std::back_inserter(json) );
  t << equal<expect, std::string>(json, "[1,2]") << FAS_FL;
  
  auto sch = method1_t::create_schema<wjrpc::default_schema>();
  
  typedef std::vector<wjrpc::default_schema> schema_list_t;
  schema_list_t schl = method_list::create_schema<wjrpc::default_schema>();
  t << equal<assert, size_t>(schl.size(), 2) << FAS_FL;
  t << flush;
  t << equal<assert, std::string>(schl[0].name, "method1") << FAS_FL;
  t << equal<assert, std::string>(schl[0].params, "[]") << FAS_FL;
  t << equal<assert, std::string>(schl[0].result, "[]") << FAS_FL;
  t << equal<assert, std::string>(schl[1].name, "method2") << FAS_FL;
  t << equal<assert, std::string>(schl[1].params, "[]") << FAS_FL;
  t << equal<assert, std::string>(schl[1].result, "[]") << FAS_FL;
}

UNIT(handler2_unit, "")
{
  using namespace fas::testing;
  using namespace wjrpc;

  std::shared_ptr<itest1> t1 = std::make_shared<test1>();
  test_count = 0;
  handler1::options_type opt;
  opt.target = t1;
  handler1 h;
  h.start(opt, 1);
  h.target() = t1;
  for (auto r: good_request)
  {
    auto req = std::make_unique< data_type>( r[0].begin(), r[0].end() );
    incoming_holder hold( std::move(req) );
    hold.parse(nullptr);
    
    h.invoke( std::move(hold), [&t,r](outgoing_holder holder)
    {
      data_ptr res = holder.detach();
      t << message("request: ") << r[0];
      t << message("response: ") << std::string(res->begin(), res->end());
      if ( std::string(res->begin(), res->end()) != r[1] )
      {
        t << ::fas::testing::error("should be:") << r[1];
      }
      ++test_count;
    });
  }
  
}

struct method_list2: wjrpc::method_list
<
  wjrpc::target<itest1>,
  wjrpc::interface_<itest1>,
  wjrpc::peeper< itest1 >,
  wjrpc::dual_method< _method1_, test1_json,      test1_json,      itest1, &itest1::method1>,
  wjrpc::dual_method< _method2_, test1_json,      test1_json,      itest1, &itest1::method2>
>
{
  virtual void method1(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback)
  {
    this->call<_method1_>(std::move(req), std::move(callback), nullptr);
  }

  virtual void method2(std::unique_ptr<test1_params> req, std::function< void(std::unique_ptr<test1_params>) > callback)
  {
    this->call<_method2_>(std::move(req), std::move(callback), nullptr);
  }
};

typedef wjrpc::handler<method_list2> handler2;

UNIT(handler4_unit, "")
{
  using namespace fas::testing;
  using namespace wjrpc;
  
  auto t1 = std::make_shared<test1>();
  handler2::options_type opt;
  opt.peeper = t1;
  
  std::string str_notify;
  
  opt.sender_handler = [&t, &str_notify]( const char* name, handler2::notify_serializer_t ser, handler2::request_serializer_t, handler2::result_handler_t ) -> void
  {
    auto d = ser(name);
    str_notify = std::string(d->begin(), d->end() );
    t << message("send: ") << str_notify;
  };

  
  auto h2 = std::make_shared<handler2>();
  h2->start(opt, 1);
  //auto h2 = std::make_shared<handler2>(nullptr, t1);
  auto p1 = std::make_unique<test1_params>(test1_params{1,2,3,4,5});
  

  h2->method1( std::make_unique<test1_params>(*p1), nullptr);
  h2->method2( std::move(p1), nullptr);
  
  t << nothing;
}


BEGIN_SUITE(handler_suite, "")
  ADD_UNIT(nohandler_unit)
  ADD_UNIT(handler1_unit)
  ADD_UNIT(gen1)
  ADD_UNIT(handler2_unit)
  ADD_UNIT(handler4_unit)
END_SUITE(handler_suite)
