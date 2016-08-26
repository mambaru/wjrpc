//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <fas/aop.hpp>
#include <wjrpc/method/aspect/tags.hpp>

namespace wjrpc{

template<typename T>
using method_group = fas::type_list< T,
                     fas::type_list< fas::group<_method_, typename T::tag>
                     > > ;

}
