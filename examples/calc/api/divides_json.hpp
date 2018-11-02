#pragma once
#include <calc/api/divides.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace request 
{
  struct divides_json
  {
    JSON_NAME(first)
    JSON_NAME(second)

    typedef wjson::object<
      divides,
      wjson::member_list<
        wjson::member<n_first, divides,  int,  &divides::first>,
        wjson::member<n_second, divides, int,  &divides::second>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

namespace response
{
  struct divides_json
  {
    JSON_NAME(value)
    typedef wjson::object<
      divides,
      wjson::member_list<
        wjson::member<n_value, divides, int, &divides::value>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}
