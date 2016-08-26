//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/basic_method.hpp>
#include <wjrpc/method/method_group.hpp>

#include <fas/aop.hpp>
#include <fas/type_list.hpp>

namespace wjrpc{

template< typename... Args>
using method = method_group< basic_method< fas::aspect<Args...> > >;

}
