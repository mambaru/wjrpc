#pragma once

#include <memory>
#include <functional>

namespace request
{
  struct plus
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<plus> ptr;
  };
} // request

namespace response
{
  struct plus 
  {
    int value=0;
    typedef std::unique_ptr<plus> ptr;
    typedef std::function<void(ptr)> callback;
  };
} // response 
