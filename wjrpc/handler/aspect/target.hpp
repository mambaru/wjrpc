//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
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


