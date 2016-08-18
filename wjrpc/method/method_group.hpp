#pragma once

#include <fas/aop.hpp>
#include <wjrpc/method/aspect/tags.hpp>

namespace wjrpc{

template<typename T>
using method_group = fas::type_list< T,
                     fas::type_list< fas::group<_method_, typename T::tag>
                     > > ;
}
