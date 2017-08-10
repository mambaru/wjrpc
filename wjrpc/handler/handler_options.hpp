//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/handler_base_options.hpp>
#include <memory>

namespace wjrpc{

template<
  typename T = std::shared_ptr< fas::empty_type>, 
  typename P = std::shared_ptr< fas::empty_type>
>
struct handler_options: handler_base_options
{
  T target; 
  P peeper;
};

} // wjrpc


