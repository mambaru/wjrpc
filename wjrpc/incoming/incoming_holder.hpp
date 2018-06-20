//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/basic_types.hpp>
#include <wjrpc/errors/errors.hpp>
#include <wjrpc/incoming/incoming.hpp>
#include <wjrpc/incoming/incoming_json.hpp>
#include <wjrpc/memory.hpp>

#include <chrono>
#include <cstring>

namespace wjrpc{

class outgoing_holder;

/**
 * @brief Предварительно распарсенное перемещаемое входящее JSONRPC-сообщение
 * @details Позволяет обрабатывать сообщение без его полной десериализации, например чтобы 
 *          определить тип сообщения, получить имя метода или идентификатор запроса. Также содержит
 *          методы для десериализации параметров. 
 * @see incoming
 */

class incoming_holder
{
public:
  /** тип обработчика исходящих сообщений */
  typedef std::function< void(outgoing_holder) > outgoing_handler_t;
  /** тип обработчика входящих сообщений */
  typedef std::function< void(incoming_holder, io_id_t, outgoing_handler_t )> incoming_handler_t;

  /** Итератор буфера сообщения */
  typedef data_type::iterator  iterator;
  /** Пара итераторов, для указания в буфере json-элементов */
  typedef std::pair< iterator, iterator> raw_t;
  
  typedef wjson::json_error json_error;
  
  /** Тип часов, для измерения времени обработки запроса */
  typedef std::chrono::high_resolution_clock clock_t;
  
  /** Временная отметка, для измерения времени обработки запроса */
  typedef clock_t::time_point time_point;

public:
  /** @brief Деструктор по умолчанию */
  ~incoming_holder() = default;
  /** @brief Деструктор по умолчанию */
  incoming_holder() = default;
  /** @brief Копирование объекта запрещено */
  incoming_holder(const incoming_holder& ) = delete;
  /** @brief Копирование объекта запрещено */
  incoming_holder& operator = (const incoming_holder& ) = delete;
  
  /** @brief Конструктор перемещения по умолчанию  */
  incoming_holder(incoming_holder&& ) = default;
  
  /** @brief Оператор перемещения по умолчанию  */
  incoming_holder& operator =(incoming_holder&& ) = default;
  
  /**
    * @brief Конструктор с захватом буфера сообщения
    * @param d буфер сообщения (`std::unique_ptr< wjrpc::data_type >`)
    * @param tp временная отметка может быть использована для замера времени прохождения запроса по системе
    * @details Просто захватывает буфер сообщения без парсинга. Для последующего анализа необходимо вызвать метод `incoming_holder::parse`
    */
  explicit incoming_holder(data_ptr    d,  time_point tp=time_point() );
  
  /**
    * @brief Конструктор с копированием буфера сообщения
    * @param d буфер сообщения (`std::vector< char >`)
    * @param tp временная отметка может быть использована для замера времени прохождения запроса по системе
    * @details Создает буфер и копирует сообщение из `d` без парсинга. Для последующего анализа необходимо вызвать метод `incoming_holder::parse`
    */
  explicit incoming_holder(data_type   d,  time_point tp=time_point() );

  /**
    * @brief Конструктор с копированием буфера сообщения
    * @param s строка с jsonrpc-сообщением
    * @param tp временная отметка может быть использована для замера времени прохождения запроса по системе
    * @details Создает буфер и копирует сообщение из строки `s` без парсинга. Для последующего анализа необходимо вызвать метод `incoming_holder::parse`
    */
  explicit incoming_holder(std::string s,  time_point tp=time_point() );

  /**
    * @brief Парсит буфер с jsonrpc-сообщением
    * @param [out] e - результат выполнения (сообщение об ошибке). Может быть nullptr.
    * @return Если в буфере более одного сообщения, то копирует оставшиеся их в отдельный буфер и возвращает. 
    * @details Производится парсинг первого json объекта в буфере. Парсинг считается успешным, 
    *          если сообщение json валидно (не обязательно быть jsonrpc). На правильность jsonrpc можно проверить 
    *          методом `incoming_holder::is_valid`. Если в буфере, за первым сообщением обнаруживается следующее, 
    *          то создается новый буфер, в него копируется остаток текущего и возвращается. 
    */
  data_ptr parse(json_error* e);
  
  /**
    * @brief Флаг того, что буфер распарсен
    * @return true - если был вызван метод `incoming_holder::parse` после создания или инициализации объекта
    * @see incoming_holder::ready;
    */
  operator bool () const;
  
  /**
    * @brief Флаг того, что буфер распарсен
    * @return true - если был вызван метод `incoming_holder::parse` после создания или инициализации объекта
    * @see incoming_holder::ready;
    */
  bool ready() const;
  
  
  /** @brief Размер буфера сообщения.  */
  size_t size() const;

  /** @brief В JSON содержит поле \b method */
  bool has_method() const;
  /** @brief В JSON содержит поле \b result */
  bool has_result() const;
  /** @brief В JSON содержит поле \b error */
  bool has_error() const;
  /** @brief В JSON содержит поле \b id */
  bool has_id() const;
  /** @brief В JSON содержит поле \b params*/
  bool has_params() const;

  /** @brief Это запрос (содержит поля \b method и \b id ) */
  bool is_request() const;
  /** @brief Это ответ на запрос (содержит поля \b result и \b id ) */
  bool is_response() const;
  /** @brief Это уведомление (содержит поле \b method, но нет \b id ) */
  bool is_notify() const;
  /** @brief Это ошибка (любая, содержит поле \b error) */
  bool is_error() const;
  /** @brief Это ошибка на запрос (содержит поля \b error и \b id) */
  bool is_request_error() const;
  /** @brief Прочие ошибки(содержит поля \b error, но нет \b id либо \b `id==nullptr`) */
  bool is_other_error() const;
  /** @brief Это запрос, ответ, уведомление или сообщение об ошибке */
  bool is_valid() const;

  /**
   * @brief Инициализирует объект или копирует в конец
   * @param d буфер сообщения (`std::unique_ptr< wjrpc::data_type >`)
   * @param tp временная отметка может быть использована для замера времени прохождения запроса по системе
   * @return \b d, если объект уже содержал буфер, либо nullptr
   * @details Если в текущем состоянии объект пуст, то захавтывает буфер. В противном случае копирует буфер \b d 
   *          в конец и возвращает \b d. Все флаги сбрасываются и необходимо вызвать вновь `incoming_holder::parse`
   */
  data_ptr attach(data_ptr d, time_point tp=time_point() );
  
  /**
   * @brief Забрать буфер 
   * @details Дальнейшая работа с объектом возможно только после вызова incoming_holder::attach
   */
  data_ptr detach();
  
  /** @brief Имя метода (если сообщение содержит поле \b method)*/
  std::string method() const;
  
  /** @brief Проверка на равенство значение поля \b method с заданной строкой */
  bool method_equal_to(const char* name) const;
  
  /** @brief Пара итераторов на начало и конец имени метода (включая кавычки)  */
  raw_t raw_method() const;
  
  /** @brief Пара итераторов на начало и конец идентификатора запроса */
  raw_t raw_id() const;

  /**
    * @brief Десериализация идентификатора запроса (поле \b id)
    * @tparam V - исходный тип идентификатора (чаще всего int) 
    * @tparam J - json-описание для идентификатора. Для простых типов достаточно значения по умолчанию 
    * @param [out] e - описание ошибки json-десериализации(м.б. nullptr)
    * @return Значение идентификатора, или V() в случае ошибки
    * @details Аналогично `incoming_holder::get_params` можно было обойтись только json-описанием, но как правило, для 
    *          идентификаторов используются простые типы, для которых запись в виде `get_id< int >` выглядит более уместной
    */
  template<typename V, typename J = ::wjson::value<V> >
  V get_id( json_error* e) const;

  
  /**
    * @brief Десериализация параметров запроса (поле \b params)
    * @tparam J - json-описание структуры параметров
    * @param [out] e - описание ошибки json-десериализации(м.б. nullptr)
    * @return `std::unique_ptr< typename J::target >` десериализованный объект 
    */
  template<typename J>
  std::unique_ptr<typename J::target> get_params( json_error* e) const;

  /**
    * @brief Десериализация параметров ответа (поле \b result)
    * @tparam J - json-описание структуры параметров
    * @param [out] e - описание ошибки json-десериализации(м.б. nullptr)
    * @return `std::unique_ptr< typename J::target >` десериализованный объект 
    */
  template<typename J>
  std::unique_ptr<typename J::target> get_result( json_error* e) const;

  /**
    * @brief Десериализация параметров ошибки (поле \b error)
    * @tparam J - json-описание структуры параметров
    * @param [out] e - описание ошибки json-десериализации(м.б. nullptr)
    * @return `std::unique_ptr< typename J::target >` десериализованный объект 
    */
  template<typename J>
  std::unique_ptr<typename J::target> get_error( json_error* e) const;

  /** @brief ссылка на данные wjrpc::incoming */
  const incoming& get() const ;
  
  /** Получить временную отметку, когда было получено сообщение (если было задано при инициализации)*/
  time_point get_time_point() const ;

  /**
   * @brief в буфере сообщения оставить только JSON-объект \b params и извлечь его
   * @return буфер с JSON-объекта с параметрами запроса
   * @details По эффекту аналогичен методу `incoming_holder::detach`
   * @see incoming_holder::detach
   */
  data_ptr acquire_params();

  /** @brief сделать дубликат текущего объекта */
  incoming_holder clone() const;

  /** @brief строка с исходным JSONRPC-сообщением */
  std::string str() const;

  /** 
   * @brief Возвращает текстовое описание ошибки с указанием места (относительно всего JSONRPC-сообщения) 
   * @details Этот метод нужно вызывать, если ошибка произошла в методе `incoming_holder::parse`
   */
  std::string error_message(const json_error& e) const;

  /** 
   * @brief Возвращает текстовое описание ошибки с указанием места (относительно секции \b params JSONRPC-сообщения) 
   * @details Этот метод нужно вызывать, если ошибка произошла в методе `incoming_holder::get_params`
   */
  std::string params_error_message(const json_error& e) const;

  /** 
   * @brief Возвращает текстовое описание ошибки с указанием места (относительно секции \b result JSONRPC-сообщения) 
   * @details Этот метод нужно вызывать, если ошибка произошла в методе `incoming_holder::get_result`
   */
  std::string result_error_message(const json_error& e) const;

  /** 
   * @brief Возвращает текстовое описание ошибки с указанием места (относительно секции \b error JSONRPC-сообщения) 
   * @details Этот метод нужно вызывать, если ошибка произошла в методе `incoming_holder::get_error`
   */
  std::string error_error_message(const json_error& e) const;

  /** 
   * @brief Возвращает текстовое описание ошибки с указанием места (относительно секции \b id JSONRPC-сообщения) 
   * @details Этот метод нужно вызывать, если ошибка произошла в методе `incoming_holder::get_id`
   */
  std::string id_error_message(const json_error& e) const;

private:

  bool ready_() const;

private:

  bool     _parsed = false;
  data_ptr _data;
  incoming _incoming;
  iterator _begin;
  iterator _end;
  time_point _time_point;
};

template<typename V, typename J >
V incoming_holder::get_id( json_error* e) const
{
  V id = V();
  if ( this->ready_() )
  {
    typename J::serializer()( id, _incoming.id.first, _incoming.id.second, e );
  }
  return id;
}

template<typename J>
std::unique_ptr<typename J::target> incoming_holder::get_result( json_error* e) const
{
  if ( !this->has_result() )
    return nullptr;
  if ( 'n'==*_incoming.result.first)
    return nullptr; // is null 
  auto result = std::make_unique<typename J::target>();

  typename J::serializer()(*result, _incoming.result.first, _incoming.result.second, e);

  if ( e && *e)
    return nullptr;

  return std::move(result);
}

template<typename J>
std::unique_ptr<typename J::target> incoming_holder::get_params( json_error* e) const
{
  if ( !this->has_params() )
    return nullptr;
  if ( 'n'==*_incoming.params.first)
    return nullptr; // is null 
  auto result = std::make_unique<typename J::target>();
  typename J::serializer()(*result, _incoming.params.first, _incoming.params.second, e);
  if ( e && *e) return nullptr;
  return std::move(result);
}

template<typename J>
std::unique_ptr<typename J::target> incoming_holder::get_error( json_error* e) const
{
  if ( !this->has_error() )
    return nullptr;
  if ( 'n'==*_incoming.error.first)
    return nullptr; // is null 
  auto result = std::make_unique<typename J::target>();
  typename J::serializer()(*result, _incoming.error.first, _incoming.error.second, e);
  if ( e && *e) return nullptr;
  return std::move(result);
}

}


