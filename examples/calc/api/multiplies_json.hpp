#pragma once
#include <calc/api/multiplies.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace request 
{
  struct multiplies_json
  {
    JSON_NAME(first)
    JSON_NAME(second)

    typedef wjson::object<
      multiplies,
      wjson::member_list<
        wjson::member<n_first, multiplies,  int,  &multiplies::first>,
        wjson::member<n_second, multiplies, int,  &multiplies::second>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}

namespace response
{
  struct multiplies_json
  {
    JSON_NAME(value)
    typedef wjson::object<
      multiplies,
      wjson::member_list<
        wjson::member<n_value, multiplies, int, &multiplies::value>
      >
    > type;
    typedef typename type::serializer serializer;
    typedef typename type::target target;
  };
}
