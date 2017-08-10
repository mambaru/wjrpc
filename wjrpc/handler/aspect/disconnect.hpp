//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace wjrpc{

template<typename Handler>
struct disconnect:
  fas::advice<_disconnect_, Handler>
{};

} // wjrpc


