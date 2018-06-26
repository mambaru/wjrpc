#include <wjrpc/incoming/incoming.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjson/_json.hpp>

#include <iostream>

/**
 * @example example01.cpp
 * @brief Пример предварительного парсинга jsonrpc-запроса в структуру wjrpc::incoming с помощью класса wjrpc::incoming_holder
 */

/**
 * Output:
      method: "plus"
      params: { "first":2, "second":3 }
      id: 1
      result: 
      error: 
*/
int main()
{
  using namespace wjson::literals;

  wjrpc::incoming_holder inholder( "{'method':'plus', 'params':{ 'first':2, 'second':3 }, 'id' :1 }"_json );
  
  // Парсим без проверок на ошибки
  inholder.parse(nullptr);

  wjrpc::incoming incom = inholder.get();
  std::cout << "method: " << std::string(incom.method.first, incom.method.second) << std::endl;
  std::cout << "params: " << std::string(incom.params.first, incom.params.second) << std::endl;
  std::cout << "id: " << std::string(incom.id.first, incom.id.second) << std::endl;
  std::cout << "result: " << std::string(incom.result.first, incom.result.second) << std::endl;
  std::cout << "error: " << std::string(incom.error.first, incom.error.second) << std::endl;
}


