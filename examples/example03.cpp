#include "calc/api/plus.hpp"
#include "calc/api/plus_json.hpp"

#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/outgoing/outgoing_result.hpp>
#include <wjrpc/outgoing/outgoing_result_json.hpp>
#include <wjson/_json.hpp>

#include <iostream>

/**
 * @example example03.cpp
 * @brief Пример "ручной" сериализации ответа на запрос 
 */

/**
 * Output:
    {"jsonrpc":"2.0","method":"plus","params":{"first":2,"second":3},"id":"id-1123"}
    {"jsonrpc":"2.0","result":{"value":5},"id":"id-1123"}
 */
int main()
{
  using namespace wjson::literals;

  wjrpc::incoming_holder inholder( "{'jsonrpc':'2.0','method':'plus','params':{'first':2,'second':3},'id':'id-1123'}"_json );
  std::cout << inholder.str() << std::endl;
  // Парсим без проверок на ошибки
  inholder.parse(nullptr);

  // Есть имя метода и идентификатор вызова
  if ( inholder.method() == "plus" )
  {
    // Десериализация параметров без проверок
    auto params = inholder.get_params<request::plus_json>(nullptr);
      
    // Объект для ответа
    wjrpc::outgoing_result<response::plus> res;
    res.result = std::make_unique<response::plus>();
    res.result->value = params->first + params->second;
    res.id = std::make_unique<wjrpc::data_type>(inholder.raw_id().first, inholder.raw_id().second);
      
    // Сериализатор ответа
    typedef wjrpc::outgoing_result_json<response::plus_json> result_json;
    result_json::serializer()( res, std::ostreambuf_iterator<char>(std::cout) );
    std::cout << std::endl;
  }
}


