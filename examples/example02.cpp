#include <wjrpc/incoming/incoming.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjson/_json.hpp>

#include <iostream>

/**
 * @example example02.cpp
 * @brief Пример простого использования wjrpc::incoming_holder, для исследования структуры запроса
 */

/**
 * Output:
      ready: 1
      size: 65
      is valid: 1
      is request: 1
      is response: 0
      is notify: 0
      is error: 0
      method: plus
      raw_method: "plus"
      method_equal_to('plus'): 1
      id: 123
      {"method":"plus", "params":{ "first":2, "second":3 }, "id" :123 }
      acquire params:{ "first":2, "second":3 }
      ready: 0 (after acquire params)
      size: 0 (after acquire params)
      is valid: 0 (after acquire params)
*/
int main()
{
  using namespace wjson::literals;

  wjrpc::incoming_holder inholder( "{'method':'plus', 'params':{ 'first':2, 'second':3 }, 'id' :123 }"_json );
  
  // Парсим без проверок на ошибки
  inholder.parse(nullptr);

  // Флаг того, что был выполнен парсинг 
  std::cout << "ready: " << inholder.ready() << std::endl;
  // рамер буфера для запроса
  std::cout << "size: " << inholder.size() << std::endl;
  // Флаг того, что был выполнен парсинг успешно
  std::cout << "is valid: " << inholder.is_valid() << std::endl;
  // Флаг того, что это запрос
  std::cout << "is request: " << inholder.is_request() << std::endl;
  // Флаг того, что это ответ (=0)
  std::cout << "is response: " << inholder.is_response() << std::endl;
  // Флаг того, что это уведомление (=0)
  std::cout << "is notify: " << inholder.is_notify() << std::endl;
  // Флаг того, что это уведомление (=0)
  std::cout << "is error: " << inholder.is_error() << std::endl;
  // Имя метода в виде строки
  std::cout << "method: " << inholder.method() << std::endl;
  // Пара итераторов для имени метода 
  auto raw = inholder.raw_method();
  std::cout << "raw_method: " << std::string(raw.first, raw.second) << std::endl;
  // Быстрый вариант проверки имени метода 
  std::cout << "method_equal_to('plus'): " << inholder.method_equal_to("plus")  << std::endl;
  // Так можно получить идентификатор 
  std::cout << "id: " << inholder.get_id<int>(nullptr)  << std::endl;
  // Получить копию буфера в виде строки
  std::cout << inholder.str()  << std::endl;
  // В буфере оставить только params и отцепить его 
  auto params = inholder.acquire_params();
  std::cout << "acquire params:"  << std::string(params->begin(), params->end() ) << std::endl;
  // Тепер inholder пуст
  std::cout << "ready: " << inholder.ready() << " (after acquire params)"<< std::endl;
  std::cout << "size: " << inholder.size() << " (after acquire params)"<< std::endl;
  std::cout << "is valid: " << inholder.is_valid() << " (after acquire params)"<< std::endl;
}


