#pragma once

#include <wjrpc/method/method_base.hpp>
#include <wjrpc/method/aspect_method.hpp>
#include <fas/aop.hpp>

namespace wjrpc{
  
template< typename... Args >
class basic_method
  : public method_base< typename fas::merge_aspect<fas::aspect<Args...>, aspect_method>::type >
{
public:
};

}
