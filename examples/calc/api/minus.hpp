#pragma once

#include <memory>
#include <functional>

namespace request
{
  struct minus
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<minus> ptr;
  };
} // request

namespace response
{
  struct minus 
  {
    int value=0;
    typedef std::unique_ptr<minus> ptr;
    typedef std::function<void(ptr)> callback;
  };
} // response 
