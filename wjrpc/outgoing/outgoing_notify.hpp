#pragma once

#include <wjrpc/outgoing/outgoing.hpp>
#include <memory>
#include <string>

namespace wjrpc{

template<typename T>
struct outgoing_notify
  : outgoing
{
  // TODO: оптимизоровать method, на const char (см. json)
  std::string method;
  std::unique_ptr<T> params;
};

}

