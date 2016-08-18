#pragma once

#include <wjrpc/handler/method_list_base.hpp>
#include <wjrpc/handler/aspect_handler.hpp>
#include <fas/aop.hpp>

namespace wjrpc{

  
template<typename... Args >
class basic_method_list
  : public method_list_base< typename fas::merge_aspect< fas::aspect<Args...>, aspect_handler>::type/*, fas::aspect_class*/ >
{
};

} // iow


