//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <wjrpc/handler/handler_options.hpp>
#include <wjrpc/method/default_schema.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/logger.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace wjrpc{

/**
 * @brief базовый класс для `wjrpc::method_list`
 * @tparam A - аспект частично определяеться в `wjrpc::method_list`, а сам список методов и цели пользователем
 */
template< typename A = fas::aspect<> >
class method_list_base
  : public ::fas::aspect_class< A >
{
  typedef ::fas::aspect_class< A > super;
  typedef method_list_base<A> self;
  typedef typename super::aspect::template advice_cast<_handler_types_>
                        ::type  handler_types;
public:

  /** Тип целевого объекта, который задан `wjrpc::target` */
  typedef typename super::aspect::template advice_cast<_target_>
                        ::type  target_type;

  /** Тип целевого объекта, который задан `wjrpc::peeper` */
  typedef typename super::aspect::template advice_cast<_peeper_>
                        ::type  peeper_type;

  /** Тип целевого объекта, который задан `wjrpc::interface_` */
  typedef typename super::aspect::template advice_cast<_interface_>
                        ::type  interface_type;

  /** Пользовательский набор данных, который может быть задан `wjrpc::context` */
  typedef typename super::aspect::template advice_cast<_context_>
                        ::type  context_type;

  /** Тип мьютекса */
  typedef typename super::aspect::template advice_cast<_mutex_type_>
                        ::type  mutex_type;

  /** Тип идентификатора поля JSONRPC запроса (поле \b id) */
  typedef typename handler_types::call_id_t        call_id_t;

  /** Тип идентификатора источника. Определяеться пользователем, по умолчанию `size_t` */
  typedef typename handler_types::io_id_t          io_id_t;

  /** Тип буфера сообщения (по умолчанию `std::vector< char >`) */
  typedef typename handler_types::data_type        data_type;

  /** Указатель на буфер сообщения (по умолчанию `std::unique_ptr< data_type >`) */
  typedef typename handler_types::data_ptr         data_ptr;

  /** Тип сообщения об ошибке, который будет сериализовыватся в случае ошибки (`wjrpc::error`)  */
  typedef typename handler_types::error_type       error_type;

  /** JSON-описание ошибки (`wjrpc::error_json`)  */
  typedef typename handler_types::error_json       error_json;

  /** по умолчанию `std::unique_ptr< error_type >`*/
  typedef typename handler_types::error_ptr        error_ptr;

  /** тип функции обратного вызова, для финальной сериализации ( по умолчанию `wjrpc::incoming_handler_t` ) */
  typedef typename handler_types::outgoing_handler_t    outgoing_handler_t;

  /** Функция сериализации запросов ( по умолчанию `wjrpc::handler_types::request_serializer_t` ) */
  typedef typename handler_types::request_serializer_t  request_serializer_t;

  /** Функция сериализации уведомлений ( по умолчанию `wjrpc::handler_types::notify_serializer_t` ) */
  typedef typename handler_types::notify_serializer_t   notify_serializer_t;

  /** Обработчик ответов для удаленных запросов( по умолчанию `wjrpc::handler_types::result_handler_t` ) */
  typedef typename handler_types::result_handler_t      result_handler_t;


  ~method_list_base()
  {
    _sender_handler = nullptr;
  }

  method_list_base()
  {
    _sender_handler = nullptr;
  }

  method_list_base(const method_list_base& ) = default;
  method_list_base(method_list_base&& ) = default;
  method_list_base& operator=(const method_list_base& ) = default;
  method_list_base& operator=(method_list_base&& ) = default;

  template<typename Tg>
  struct call_params_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::params_ptr type;
  };

  template<typename Tg>
  struct call_result_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::result_ptr type;
  };

  template<typename Tg>
  struct call_error_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::error_ptr type;
  };

  /**
   * @brief вызов "удаленного" метода
   * @tparam Tg тэг и имя метода
   * @tparam ReqPtr тип указателя на запрос
   * @param req параметры запроса, определяеться в `wjrpc::call_method`
   * @param callback обработчик ответа
   * @details Сериализует запрос
   *
   */
  template<typename Tg, typename ReqPtr>
  void call(
    ReqPtr req,
    std::function< void(
      typename call_result_ptr<Tg>::type,
      typename call_error_ptr<Tg>::type
    )> callback
  ) const
  {
    this->get_aspect().template get<Tg>()
                      .call( *this, std::move(req), std::move(callback) );
  }

  template<typename Tg, typename ReqPtr>
  void call(
    ReqPtr req,
    std::function<void(typename call_result_ptr<Tg>::type)> result_callback,
    std::function<void(typename call_error_ptr<Tg>::type)>  error_callback
  ) const
  {
    typedef std::function<void(
      typename call_result_ptr<Tg>::type,
      typename call_error_ptr<Tg>::type
    )> callback_type;

    using namespace std::placeholders;
    callback_type callback = nullptr;
    if ( result_callback!=nullptr )
      callback = std::bind( self::response_handler<Tg>, _1, _2, result_callback, error_callback );

    this->get_aspect().template get<Tg>().call(
      *this,
      std::move(req),
      std::move(callback)
    );
  }

  template<typename Params, typename NotifySerializer, typename RequestSerializer, typename ResultHandler >
  void perform_send(
    const char* method_name,
    Params params,
    NotifySerializer ns,
    RequestSerializer rs,
    ResultHandler  rh
  ) const
  {
    super::get_aspect().template get<_perform_send_>()(
      *this,
      method_name,
      std::move(params),
      std::move(ns),
      std::move(rs),
      std::move(rh)
    );
  }

  void sender_handler( const char* method_name, notify_serializer_t ns1, request_serializer_t rs1, result_handler_t rh1) const
  {
    if ( this->_sender_handler != nullptr )
    {
      this->_sender_handler( method_name, std::move(ns1), std::move(rs1), std::move(rh1) );
    }
    else
    {
      WJRPC_LOG_FATAL(" (ABORT) wjrpc::jsonrpc::method_list_base::sender_handler this->_sender_handler==nullptr")
    }
  }

  template<typename Schema>
  static std::vector<Schema> create_schema_t(const std::vector<std::string>& names)
  {
    typedef typename super::aspect::template select_group<_method_>::type method_tag_list_t;
    std::vector<Schema> sch_list;
    create_schema_(&sch_list, names, method_tag_list_t());
    return sch_list;
  }

  typedef std::vector<default_schema> schema_list_t;

  static schema_list_t create_schema(const std::vector<std::string>& names)
  {
    return create_schema_t<default_schema>(names);
  }

private:

  template<typename Schema, typename L, typename R>
  static void create_schema_(std::vector<Schema>* sch_list, const std::vector<std::string>& names, fas::type_list<L, R>)
  {
    typedef L head_tag;
    typedef R tail_type;
    typedef typename super::aspect::template advice_cast<head_tag>::type method_type;
    sch_list->push_back( method_type::template create_schema_t<Schema>(names) );
    create_schema_(sch_list, names, tail_type() );
  }

  template<typename Schema>
  static void create_schema_(std::vector<Schema>*,  const std::vector<std::string>&, fas::empty_list)
  {
  }



  template<typename Tg>
  static inline void response_handler(
    typename call_result_ptr<Tg>::type res,
    typename call_error_ptr<Tg>::type err,
    std::function<void(typename call_result_ptr<Tg>::type)> result_callback,
    std::function<void(typename call_error_ptr<Tg>::type)> error_callback
  )
  {
    self::aspect::template advice_cast<_response_handler_>::type
        ::template process<Tg>(
            std::move(res),
            std::move(err),
            std::move(result_callback),
            std::move(error_callback)
        );
  }

private:
  friend typename super::aspect::template advice_cast< _initialize_ >::type;
  typedef typename handler_types::sender_handler_t   sender_handler_t;
  sender_handler_t _sender_handler = sender_handler_t(nullptr);
};

} // wjrpc


