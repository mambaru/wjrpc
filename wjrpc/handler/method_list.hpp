//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/basic_method_list.hpp>

namespace wjrpc{

template<typename... Args >
class method_list
  : public basic_method_list<Args...>
{
public:
  typedef basic_method_list<Args...> super;
  typedef typename basic_method_list<Args...>::interface_type interface_type;
  typedef typename basic_method_list<Args...>::handler_types handler_types;
};


} // wjrpc


