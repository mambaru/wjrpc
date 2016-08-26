//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/method_base.hpp>
#include <wjrpc/method/aspect_method.hpp>
#include <fas/aop.hpp>

namespace wjrpc{
  
template< typename... Args >
class basic_method
  : public method_base< typename fas::merge_aspect<fas::aspect<Args...>, aspect_method>::type >
{
public:
};

}
