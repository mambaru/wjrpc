//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop/metalist.hpp>

namespace wjrpc{
  
template<typename I>
struct interface_ 
{
  typedef fas::metalist::advice metatype;
  typedef _interface_ tag;
  typedef I advice_class;
};

} // wjrpc


