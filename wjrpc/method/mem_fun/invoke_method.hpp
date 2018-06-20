//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/name.hpp>
#include <wjrpc/method/mem_fun/invoke_mem_fun.hpp>
#include <wjrpc/method/method.hpp>
#include <memory>
#include <functional>


namespace wjrpc{
  
template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ),
  typename ...Args
>
struct basic_invoke_method
  : basic_method< 
      name<TgName>,
      invoke_mem_fun< 
        JParams,
        JResult,
        I,
        mem_ptr
      >,
      Args...
    >
{};


/**
 * @brief конструкция для описания обработчика конкретного метода 
 * @tparam TgName Имя метода. Задатеться с помощью `JSONRPC_TAG`
 * @tparam JParams JSON-описание параметров запроса
 * @tparam JResult JSON-описание структуры ответа 
 * @tparam I тип целевого класса (интерфейса), которые был задан в `wjrpc::target`
 * @tparam mem_ptr указатель на функцию-член класс I
 * @tparam Args дополнительные параметры обработчика метода
 * @details С помощью таких конструкций определяеться списко методов для `wjrpc::member_list`
 */
template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ),
  typename ...Args
>
struct invoke_method
  : method< 
      name<TgName>,
      invoke_mem_fun< 
        JParams,
        JResult,
        I,
        mem_ptr
      >,
      Args...
    >
{};

} // wjrpc
