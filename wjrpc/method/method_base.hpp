//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/logger.hpp>
#include <wjson/schema.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>


namespace wjrpc{

template<
  typename A = fas::aspect<>,
  template<typename> class AspectClass = fas::aspect_class
>
class method_base
  : public AspectClass<A>
{
public:
  typedef method_base<A> self;
  typedef AspectClass<A> super;

  typedef ::wjrpc::data_type data_type;
  typedef ::wjrpc::data_ptr  data_ptr;

  typedef fas::metalist::advice metatype;
  typedef typename super::aspect::template advice_cast<_name_>::type::name_type tag;
  typedef self advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  const char* name() const
  {
    return this->get_aspect().template get<_name_>()();
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  template<typename T>
  void operator()(
    T& t,
    incoming_holder holder,
    std::function<void(outgoing_holder)> outgoing_handler
  )
  {
    this->get_aspect().template get<_invoke_>()(
      t,
      *this,
      std::move(holder),
      std::move(outgoing_handler)
    );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  template<typename T, typename ParamsPtr, typename Callback>
  void call(T& t, ParamsPtr params, Callback callback) const
  {

    this->get_aspect().template get<_call_>()(
      t,
      *this,
      std::move(params),
      std::move(callback)
    );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  template<typename T, typename ParamsJson>
  static inline auto serialize_request(
    const char* name,
    std::unique_ptr<typename ParamsJson::target> req,
    typename T::call_id_t id
  ) -> typename T::data_ptr
  {
    return super::aspect::template advice_cast<_request_serializer_>
                ::type::template serialize<T, ParamsJson>(
                    name,
                    std::move(req),
                    id
                );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  template<typename T, typename ParamsJson>
  static inline auto serialize_notify(
    const char* name,
    std::unique_ptr<typename ParamsJson::target> req
  ) -> typename T::data_ptr
  {
    return super::aspect::template advice_cast<_notify_serializer_>::type
                ::template serialize<T, ParamsJson>( name, std::move(req) );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  template<typename T, typename ResultJson, typename ErrorJson>
  static inline void process_response(
    incoming_holder holder,
    std::function< void (
      std::unique_ptr<typename ResultJson::target>,
      std::unique_ptr<typename ErrorJson::target>
    )> callback
  )
  {
    return super::aspect::template advice_cast<_process_response_>::type
            ::template process<T, ResultJson, ErrorJson>(
                std::move(holder),
                std::move(callback)
              );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////


  template<typename T, typename ResultJson>
  static inline void send_result(
    incoming_holder holder,
    std::unique_ptr<typename ResultJson::target> result,
    std::function<void(outgoing_holder)> outgoing_handler
  )
  {
    return super::aspect::template advice_cast<_send_result_>::type
            ::template send<T, ResultJson>(
                std::move(holder),
                std::move(result),
                std::move(outgoing_handler)
            );
  }

  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////


  template<typename T, typename ErrorJson>
  static inline void send_error(
    incoming_holder holder,
    std::unique_ptr<typename ErrorJson::target> err,
    std::function<void(outgoing_holder)> outgoing_handler
  )
  {
    return super::aspect::template advice_cast<_send_error_>::type
            ::template send<T, ErrorJson>(
                std::move(holder),
                std::move(err),
                std::move(outgoing_handler)
            );

  }


  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////
  /// ///////////////////////////////////////////////////

  // Сначала проверяем _call_, т.к. на _invoke_ по умолчанию стоит заглушка "method not impl"
  typedef typename fas::if_c<
    super::aspect::template has_advice<_call_>::value,
    _call_,
    _invoke_
  >::type schema_tag;

  typedef typename super::aspect::template advice_cast<schema_tag>::type schema_advice;

  static void get_schema_list(std::vector<std::string>& schemas)
  {
    typedef typename schema_advice::params_type params_type_t;
    typedef typename schema_advice::result_type result_type_t;
    typedef typename schema_advice::error_type error_type_t;
    wjson::schema::get_schema_list<params_type_t>(schemas);
    wjson::schema::get_schema_list<result_type_t>(schemas);
    wjson::schema::get_schema_list<error_type_t>(schemas);
  }

  template<
    typename Schema, // default_schema
    typename Params,
    typename Result,
    typename Error
  >
  static Schema create_schema_t(const Params& params, const Result& result, const Error& error)
  {
    typedef typename schema_advice::params_json_t::serializer params_serializer;
    typedef typename schema_advice::result_json_t::serializer result_serializer;
    typedef typename schema_advice::error_json_t::serializer  error_serializer;

    typedef typename super::aspect::template advice_cast<_name_>::type name_advice;

    Schema sch;
    const char* n = name_advice()();
    std::copy(n, n + std::strlen(n), std::back_inserter(sch.name) );
    params_serializer()(params, std::back_inserter(sch.params));
    result_serializer()(result, std::back_inserter(sch.result));
    error_serializer()(error, std::back_inserter(sch.error));
    return sch;
  }

  template<
    typename Schema,
    typename Params,
    typename Result
  >
  static Schema create_schema_t(const Params& params, const Result& result, const std::vector<std::string>& names)
  {
    typedef typename schema_advice::error_type error_type_t;
    error_type_t error;
    wjson::schema::create_schema(error, names);
    return create_schema_t<Schema>(params, result, error);
  }

  template<
    typename Schema,
    typename Params
  >
  static Schema create_schema_t(const Params& params, const std::vector<std::string>& names)
  {
    typedef typename schema_advice::result_type result_type_t;
    result_type_t result;
    wjson::schema::create_schema(result, names);
    return create_schema_t<Schema>(params, result, names);
  }

  template<typename Schema>
  static Schema create_schema_t(const std::vector<std::string>& names)
  {
    typedef typename schema_advice::params_type params_type_t;
    params_type_t params;
    wjson::schema::create_schema(params, names);
    return create_schema_t<Schema>(params, names);
  }
};

}
