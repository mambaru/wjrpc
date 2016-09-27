#pragma once
#include "calc/api/plus.hpp"
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace request 
{
  struct plus_json
  {
    JSON_NAME(first)
    JSON_NAME(second)

    typedef wjson::object<
      plus,
      wjson::member_list<
        wjson::member<n_first, plus,  int,  &plus::first>,
        wjson::member<n_second, plus, int,  &plus::second>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

namespace response
{
  struct plus_json
  {
    JSON_NAME(value)
    typedef wjson::object<
      plus,
      wjson::member_list<
        wjson::member<n_value, plus, int, &plus::value>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}
