#include <wjrpc/handler.hpp>
#include <wjrpc/engine.hpp>
#include <wjrpc/method.hpp>
#include <fas/testing.hpp>

#include <algorithm>
#include <memory>

// *********************************
//              Maker
// *********************************

// request maker
namespace request
{
  template<typename T>
  struct basic 
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<T> ptr;
  };

  template<typename T>
  struct basic_json
  {
    FAS_NAME(first)
    FAS_NAME(second)

    typedef ::wjson::object<
      T,
      ::wjson::member_list<
        ::wjson::member<n_first, basic<T>,  int,  &basic<T>::first>,
        ::wjson::member<n_second, basic<T>, int,  &basic<T>::second>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

// response maker
namespace response
{
  template<typename T>
  struct basic
  {
    int value=0;
    typedef std::unique_ptr<T> ptr;
    typedef std::function<void(ptr)> callback;
  };

  template<typename T>
  struct basic_json
  {
    FAS_NAME(value)

    typedef ::wjson::object<
      T,
      ::wjson::member_list<
        ::wjson::member<n_value, basic<T>, int, &basic<T>::value>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

// *********************************
//              Interface
// *********************************

namespace request
{
  struct plus:  basic<plus> {};
  struct minus: basic<minus> {};
  struct multiplies: basic<multiplies> {};
  struct divides: basic<divides> {}; 
}

namespace response
{
  struct plus:  basic<plus> {};
  struct minus: basic<minus> {};
  struct multiplies: basic<multiplies> {};
  struct divides: basic<divides> {}; 
}

struct icalc
{
  virtual ~icalc() {}
  virtual void plus( request::plus::ptr req, response::plus::callback cb) = 0;
  virtual void minus( request::minus::ptr req, response::minus::callback cb) = 0;
  virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) = 0;
  virtual void divides( request::divides::ptr req, response::divides::callback cb) = 0;
};


// *********************************
//              Domain
// *********************************

class calc
  : public icalc
{
public:
  virtual void plus( request::plus::ptr req, response::plus::callback cb) override;
  virtual void minus( request::minus::ptr req, response::minus::callback cb) override;
  virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) override;
  virtual void divides( request::divides::ptr req, response::divides::callback cb) override;
private:
  template<typename Res, typename ReqPtr, typename Callback, typename F>
  void impl_( ReqPtr req, Callback cb, F f);
};

template<typename Res, typename ReqPtr, typename Callback, typename F>
void calc::impl_( ReqPtr req, Callback cb, F f)
{
  if ( cb == nullptr )
    return;

  if ( req == nullptr )
    return cb(nullptr);

  auto res = std::make_unique<Res>();
  res->value = f(req->first,req->second);
  cb( std::move(res) );
}

void calc::plus( request::plus::ptr req, response::plus::callback cb)
{
  this->impl_<response::plus>( std::move(req), cb, [](int f, int s) { return f+s; } );
}

void calc::minus( request::minus::ptr req, response::minus::callback cb)
{
  this->impl_<response::minus>( std::move(req), cb, [](int f, int s) { return f-s; });
}

void calc::multiplies( request::multiplies::ptr req, response::multiplies::callback cb)
{
  this->impl_<response::multiplies>( std::move(req), cb, [](int f, int s) { return f*s; });
}

void calc::divides( request::divides::ptr req, response::divides::callback cb)
{
  this->impl_<response::divides>( std::move(req), cb, [](int f, int s) { return s!=0 ? f/s : 0; });
}

// *********************************
//              JSON
// *********************************

namespace request
{
  struct plus_json: basic_json<plus> {};
  struct minus_json: basic_json<minus> {};
  struct multiplies_json: basic_json<multiplies> {};
  struct divides_json: basic_json<divides> {};
}

namespace response
{
  struct plus_json: basic_json<plus> {};
  struct minus_json: basic_json<minus> {};
  struct multiplies_json: basic_json<multiplies> {};
  struct divides_json: basic_json<divides> {};
}

// *********************************
//              JSON-RPC
// *********************************

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
typedef ::wjrpc::engine< handler > engine_type;

// *********************************
//              Engine
// *********************************

class demo
{
public:
  demo()
  {
    _jsonrpc = std::make_shared<engine_type>();
    _calc = std::make_shared<calc>();
    engine_type::options_type opt;
    opt.target = _calc;
    _jsonrpc->start(opt, 1);
  }

  void perform(std::string str, ::wjrpc::io_id_t io_id = 2)
  {
    auto data = std::make_unique< wjrpc::data_type >(str.begin(), str.end());
    _jsonrpc->perform_io( std::move(data), io_id, [str]( wjrpc::data_ptr d) 
    {
      std::cout << "request: " << str << std::endl;
      std::cout << "responce: " << std::string( d->begin(), d->end() ) << std::endl;
      std::cout << std::endl;
    });

  }
private:
  std::shared_ptr<engine_type> _jsonrpc;
  std::shared_ptr<calc> _calc;
};

int main()
{
  demo dm;
  dm.perform("{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}");
  dm.perform("{\"method\":\"minus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}");
  dm.perform("{\"method\":\"multiplies\",\"params\":{\"first\":2, \"second\":3},\"id\":1}");
  dm.perform("{\"method\":\"divides\",\"params\":{\"first\":2, \"second\":3},\"id\":1}");
}
