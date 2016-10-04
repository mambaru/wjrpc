#pragma once

#include <wjrpc/types.hpp>
#include <atomic>

#ifndef NDEBUG
#define LOG_READ(BEG, END) std::clog << getpid() << " READ: " << std::string(BEG, END) << std::endl;
#define LOG_WRITE(BEG, END) std::clog << getpid() << " WRITE: " << std::string(BEG, END) << std::endl;
#else
#define LOG_READ(BEG, END)
#define LOG_WRITE(BEG, END)
#endif

inline wjrpc::io_id_t create_id()
{
  static std::atomic<wjrpc::io_id_t> counter( (wjrpc::io_id_t(1)) );
  return counter.fetch_add(1);
}
