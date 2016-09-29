#pragma once

#include <memory>
#include <functional>

namespace request
{
  struct divides
  {
    int first=0; 
    int second=0;
    typedef std::unique_ptr<divides> ptr;
  };
} // request

namespace response
{
  struct divides
  {
    int value=0;
    typedef std::unique_ptr<divides> ptr;
    typedef std::function<void(ptr)> callback;
  };
} // response 
