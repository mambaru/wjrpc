//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/handler_base_options.hpp>
#include <memory>

namespace wjrpc{

template<typename T, typename P>
struct handler_options: handler_base_options
{
  T target; 
  P peeper;
};

} // iow


