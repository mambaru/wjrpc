
#pragma once

#include "icalc.hpp"

class calc_p
  : public icalc
{
public:
  void initialize(std::shared_ptr<icalc>);
  virtual void plus( request::plus::ptr req, response::plus::callback cb) override;
  virtual void minus( request::minus::ptr req, response::minus::callback cb) override;
  virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) override;
  virtual void divides( request::divides::ptr req, response::divides::callback cb) override;
private:
  template<typename ReqPtr, typename Callback>
  static bool check_( ReqPtr& req, Callback& cb);
  std::shared_ptr<icalc> _next;
};
