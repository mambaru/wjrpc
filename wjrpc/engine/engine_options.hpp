#pragma once
#include <wjrpc/types.hpp>
#include <wjrpc/logger.hpp>

namespace wjrpc{
  
template<typename HandlerOptions>
struct engine_options
  : HandlerOptions
{
  typedef HandlerOptions handler_options;
  // максимальное время ожидания ответа на вызов (0 - неограничено)
  time_t call_lifetime_ms = 60000;
  bool   remove_everytime = true;
  bool   disable_handler_map = false;
};

}
