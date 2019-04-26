#pragma once

#include "icalc.hpp"

class calc1 final
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
