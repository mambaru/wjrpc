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

} // iow


