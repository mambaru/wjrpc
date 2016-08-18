#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wjrpc{
  
template<typename I>
struct target
  : fas::value<_target_, std::shared_ptr<I> >
{
};

} 


