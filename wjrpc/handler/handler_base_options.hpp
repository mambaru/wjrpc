//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/handler_types.hpp>
#include <wjrpc/logger.hpp>

namespace wjrpc{

struct handler_base_options
{
  typedef handler_types::sender_handler_t sender_handler_t;
  sender_handler_t sender_handler;
};

} // wjrpc


