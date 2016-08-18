#pragma once

#include <wjrpc/handler/handler_types.hpp>
#include <wjrpc/logger.hpp>

namespace wjrpc{

struct handler_base_options:
  ::wjrpc::logger_options
{
  typedef handler_types::sender_handler_t sender_handler_t;
  sender_handler_t sender_handler;
};

} // iow


