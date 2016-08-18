#pragma once

#include <wjrpc/basic_types.hpp>
#include <utility>

namespace wjrpc{

struct incoming
{
  typedef data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  pair_type method;
  pair_type params;
  pair_type result;
  pair_type error;
  pair_type id;
};

}
