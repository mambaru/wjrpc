#pragma once

#include <memory>
#include <functional>

namespace request
{
  struct multiplies
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<multiplies> ptr;
  };
} // request

namespace response
{
  struct multiplies 
  {
    int value=0;
    typedef std::unique_ptr<multiplies> ptr;
    typedef std::function<void(ptr)> callback;
  };
} // response 
