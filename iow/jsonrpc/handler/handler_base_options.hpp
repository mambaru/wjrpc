#pragma once

#include <iow/jsonrpc/handler/handler_types.hpp>
#include <iow/jsonrpc/logger.hpp>

namespace iow{ namespace jsonrpc{

struct handler_base_options:
  ::wjrpc::logger_options
{
  typedef handler_types::sender_handler_t sender_handler_t;
  sender_handler_t sender_handler;
};

}} // iow


