#include <wjrpc/handler.hpp>
#include <wjrpc/engine.hpp>
#include <wjrpc/method.hpp>
#include <fas/testing.hpp>

#include <algorithm>
#include <memory>

// *********************************
//              Interface
// *********************************
namespace request
{
  struct plus
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<plus> ptr;
  };
} // request

namespace response
{
  struct plus 
  {
    int value=0;
    typedef std::unique_ptr<plus> ptr;
    typedef std::function<void(ptr)> callback;
  };
} // response 


struct icalc
{
  virtual ~icalc() {}
  virtual void plus( request::plus::ptr req, response::plus::callback cb) = 0;
};

// *********************************
//              Domain
// *********************************

class calc
  : public icalc
{
public:
  virtual void plus( request::plus::ptr req, response::plus::callback cb) override;
};

void calc::plus( request::plus::ptr req, response::plus::callback cb)
{
  if ( cb == nullptr )
    return;

  if ( req == nullptr )
    return cb(nullptr);

  auto res = std::make_unique<response::plus>();
  res->value = req->first + req->second;
  cb( std::move(res) );
}

// *********************************
//              JSON
// *********************************

namespace request 
{
  struct plus_json
  {
    FAS_NAME(first)
    FAS_NAME(second)

    typedef wjson::object<
      plus,
      wjson::member_list<
        wjson::member<n_first, plus,  int,  &plus::first>,
        wjson::member<n_second, plus, int,  &plus::second>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

namespace response
{
  struct plus_json
  {
    FAS_NAME(value)
    typedef wjson::object<
      plus,
      wjson::member_list<
        wjson::member<n_value, plus, int, &plus::value>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

// *********************************
//              JSON-RPC
// *********************************

JSONRPC_TAG(plus)

struct method_list: wjrpc::method_list
<
  wjrpc::target<icalc>,
  wjrpc::invoke_method<_plus_, request::plus_json, response::plus_json, icalc, &icalc::plus>
>{};


// *********************************
//              Engine
// *********************************

class handler: public ::wjrpc::handler<method_list> {};

int main()
{
  typedef ::wjrpc::engine< handler > engine_type;
    engine_type::options_type opt;
  auto pcalc = std::make_shared<calc>();
  opt.target = pcalc;
  auto e = std::make_shared<engine_type>();
  e->start(opt, 1);
  std::string sreq = "{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}";
  e->perform_io( std::make_unique< wjrpc::data_type >(sreq.begin(), sreq.end()), 1, []( wjrpc::data_ptr d) 
  {
    using namespace ::fas::testing;
    std::cout << "responce: " << std::string( d->begin(), d->end() ) << std::endl;
  });
}


