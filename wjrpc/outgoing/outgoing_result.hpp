//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/outgoing/outgoing.hpp>
#include <wjrpc/basic_types.hpp>

namespace wjrpc{

template<typename T>
struct outgoing_result
  : outgoing
{
  std::unique_ptr<T> result;
  data_ptr id;
};

  
}

