#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace wjrpc{

template<typename Handler>
struct connect:
  fas::advice<_connect_, Handler>
{};

} // iow


