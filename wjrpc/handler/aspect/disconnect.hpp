#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace wjrpc{

template<typename Handler>
struct disconnect:
  fas::advice<_disconnect_, Handler>
{};

} // iow


