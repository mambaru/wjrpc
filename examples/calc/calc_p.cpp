#include "calc_p.hpp"
#include <memory>

void calc_p::initialize(std::shared_ptr<icalc> next)
{
  _next = next;
}

void calc_p::plus( request::plus::ptr req, response::plus::callback cb)
{
  if ( !this->check_(req, cb))
    return;
  req->first++;
  req->second++;
  _next->plus(std::move(req), [cb](response::plus::ptr res)
  {
    res->value++;
    cb(std::move(res) );
  });
}

void calc_p::minus( request::minus::ptr req, response::minus::callback cb)
{
  if ( this->check_(req, cb))
    _next->minus(std::move(req), std::move(cb) );
}

void calc_p::multiplies( request::multiplies::ptr req, response::multiplies::callback cb)
{
  if ( this->check_(req, cb))
    _next->multiplies(std::move(req), std::move(cb) );
}

void calc_p::divides( request::divides::ptr req, response::divides::callback cb)
{
  if ( this->check_(req, cb))
    _next->divides(std::move(req), std::move(cb) );
}

template<typename ReqPtr, typename Callback>
bool calc_p::check_( ReqPtr& req, Callback& cb)
{
  if ( cb==nullptr )
    return false;
  if ( req != nullptr )
    return true;
  cb(nullptr);
  return false;
}
