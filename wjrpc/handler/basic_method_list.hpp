//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/method_list_base.hpp>
#include <wjrpc/handler/aspect_handler.hpp>
#include <fas/aop.hpp>

namespace wjrpc{

  
template<typename... Args >
class basic_method_list
  : public method_list_base< typename fas::merge_aspect< fas::aspect<Args...>, aspect_handler>::type/*, fas::aspect_class*/ >
{
};

} // iow


