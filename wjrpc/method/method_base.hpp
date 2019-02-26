//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/method/aspect/tags.hpp>
#include <wjrpc/logger.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>

#define FAS_HAS_MEMBER_FUNCTION0P(N, D)\
namespace N##_detail{\
template < typename T, typename R, R(*)() > struct op_mf {}; \
struct N##_helper {\
  typedef char small_type;\
  typedef struct big_tag { char dummy[2];} big_type;\
  template<typename P, typename R>\
  static small_type test(op_mf<P, R, &P::D>*);\
  template<typename P, typename R>\
  static big_type test(...);\
}; \
} \
template<typename T, typename R>\
struct N\
{\
  typedef N##_detail::N##_helper helper; \
  enum { value = sizeof( helper::test<T, R>(0) ) == sizeof(helper::small_type) };\
};

FAS_HAS_MEMBER_FUNCTION0P(has_schema_creator, create_schema)

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

  advice_class& get_advice() { return *this;};
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
  static Schema create_schema_t(const Params& params, const Result& result)
  {
    typedef typename schema_advice::error_type error_type;
    error_type error = self::template create_value<error_type>();
    return create_schema_t<Schema>(params, result, error);
  }

  template<
    typename Schema, 
    typename Params
  >
  static Schema create_schema_t(const Params& params)
  {
    typedef typename schema_advice::result_type result_type;
    result_type result = self::template create_value<result_type>();
    return create_schema_t<Schema>(params, result);
  }

  template<typename Schema>
  static Schema create_schema_t()
  {
    typedef typename schema_advice::params_type params_type;
    params_type params = self::template create_value<params_type>();
    return create_schema_t<Schema>(params);
  }
  
  template<typename V>
  static V create_value()
  {
    V value = V();
    generate_value_(&value, fas::bool_<has_schema_creator<V, V>::value>() );
    return value;
  }

private:
  template<typename V>
  static void generate_value_(V*, fas::false_) { }

  template<typename V>
  static void generate_value_(V* value, fas::true_){ *value = V().create_schema(); }
};

}
