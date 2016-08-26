//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>

namespace wjrpc{
  
template<typename N>
struct name
{
  typedef fas::metalist::advice metatype;
  typedef _name_ tag;
  typedef name<N> advice_class;
  typedef N name_type;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* operator()() const
  {
    return name_type()();
  }
};

} // iow


