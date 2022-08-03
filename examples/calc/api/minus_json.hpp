#pragma once
#include <calc/api/minus.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace request 
{
  struct minus_json
  {
    JSON_NAME(first)
    JSON_NAME(second)

    typedef wjson::object<
      minus,
      wjson::member_list<
        wjson::member<n_first, minus,  int,  &minus::first>,
        wjson::member<n_second, minus, int,  &minus::second>
      >
    > type1;
    typedef typename type1::serializer serializer;
    typedef typename type1::target target;
  };
}

namespace response
{
  struct minus_json
  {
    JSON_NAME(value)
    typedef wjson::object<
      minus,
      wjson::member_list<
        wjson::member<n_value, minus, int, &minus::value>
      >
    > type1;
    typedef typename type1::serializer serializer;
    typedef typename type1::target target;
  };
}
