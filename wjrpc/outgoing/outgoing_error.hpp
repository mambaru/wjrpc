#pragma once

#include <wjrpc/outgoing/outgoing.hpp>
#include <wjrpc/basic_types.hpp>
#include <memory>

namespace wjrpc{

template<typename T>
struct outgoing_error: outgoing
{
  std::unique_ptr<T> error;
  std::unique_ptr<data_type> id;
  outgoing_error()
    : outgoing()
    , id(nullptr)
  {}
};
}

