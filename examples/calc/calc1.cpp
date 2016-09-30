#include "calc1.hpp"
#include <wjrpc/memory.hpp>

template<typename Res, typename ReqPtr, typename Callback, typename F>
void calc1::impl_( ReqPtr req, Callback cb, F f)
{
  if ( cb == nullptr )
    return;

  if ( req == nullptr )
    return cb(nullptr);

  auto res = std::make_unique<Res>();
  res->value = f(req->first,req->second);
  cb( std::move(res) );
}

void calc1::plus( request::plus::ptr req, response::plus::callback cb)
{
  this->impl_<response::plus>( std::move(req), cb, [](int f, int s) { return f+s; } );
}

void calc1::minus( request::minus::ptr req, response::minus::callback cb)
{
  this->impl_<response::minus>( std::move(req), cb, [](int f, int s) { return f-s; });
}

void calc1::multiplies( request::multiplies::ptr req, response::multiplies::callback cb)
{
  this->impl_<response::multiplies>( std::move(req), cb, [](int f, int s) { return f*s; });
}

void calc1::divides( request::divides::ptr req, response::divides::callback cb)
{
  this->impl_<response::divides>( std::move(req), cb, [](int f, int s) { return s!=0 ? f/s : 0; });
}
