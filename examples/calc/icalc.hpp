#pragma once

#include "api/plus.hpp"
#include "api/minus.hpp"
#include "api/multiplies.hpp"
#include "api/divides.hpp"

class icalc
{
public:
  virtual ~icalc() = default;
  icalc() = default;
  icalc(const icalc&) = default;
  icalc(icalc&&) = default;
  icalc& operator=(const icalc&) = default;
  icalc& operator=(icalc&&) = default;

  virtual void plus( request::plus::ptr req, response::plus::callback cb) = 0;
  virtual void minus( request::minus::ptr req, response::minus::callback cb) = 0;
  virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) = 0;
  virtual void divides( request::divides::ptr req, response::divides::callback cb) = 0;
};

