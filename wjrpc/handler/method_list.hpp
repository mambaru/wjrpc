//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/method_list_base.hpp>
#include <wjrpc/handler/aspect_handler.hpp>
#include <fas/aop.hpp>

namespace wjrpc{

/**
 * @brief JSONRPC обработчик. 
 * @tparam MethodList список методов сформированный с помощью `wjrpc::method_list`
 * @details Принимает на вход строку (`wjrpc::data_ptr`) или предварительно распарсенное сообщение (`wjrpc::incoming_holder`), 
 *          определяет его тип, имя метода и вызывает его обработчик. Основной функционал реализован в списке методов, который можно сформировать 
 *          с помощью `wjrpc::method_list` и передать в качестве шаблонного параметра
 */
template<typename... Args >
class method_list
  : public method_list_base< typename fas::merge_aspect< fas::aspect<Args...>, aspect_handler>::type >
{
};

} // wjrpc


