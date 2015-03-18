#pragma once

#include <iow/io/basic/tags.hpp>

namespace iow{ namespace io{ namespace basic{
  
struct ad_reset
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_owner_>().reset();
    t.get_aspect().template gete<_after_reset_>()(t);
  }
};
  
}}}
