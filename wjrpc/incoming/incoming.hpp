//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/basic_types.hpp>
#include <utility>

namespace wjrpc{

/**
 * @brief Результаты предварительного парсинга входящего JSONRPC сообщения. 
 * @details Содержит пары итераторов, указывающих на начало и 
 *        окончание элементов сообщения во входном буфере. Если соответствующего элемента нет во входящем запросе, то `first==second`.
 *        По комбинации полей сообщения можно определить его тип: запрос, уведомление, ответ или сообщение об ошибке. Предполагается, 
 *        что сообщение в текстовом виде хранится в буфере `wjrpc::data_type` (`std::vector< char>`), а во время предварительного парсинга
 *        заполняется эта структура, которая может использоваться для выполнения логики, где не требуется полная десериализация сообщения.
 * @see incoming_holder
  */
struct incoming
{
  /** Итератор входного буфера сообщения. */
  typedef data_type::iterator iterator;
  /** Пара итераторов входного буфера сообщения. */
  typedef std::pair<iterator, iterator> pair_type;
  /** Значение поля \b method. Только в запросах и уведомлениях.*/
  pair_type method;
  /** Значение поля \b params. Только в запросах и уведомлениях.*/
  pair_type params;
  /** Значение поля \b result. Только в ответах на запрос.*/
  pair_type result;
  /** Значение поля \b error. Только в сообщениях об ошибке. */
  pair_type error;
  /** Значение поля \b id. Только в запросах и ответах на запрос. */
  pair_type id;
};

}
