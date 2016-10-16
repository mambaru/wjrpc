#pragma once

#include <wjrpc/basic_types.hpp>
#include <wjrpc/errors/errors.hpp>
#include <wjrpc/incoming/incoming.hpp>
#include <wjrpc/incoming/incoming_json.hpp>
#include <wjrpc/memory.hpp>

#include <chrono>
#include <cstring>

//#define IOW_JSON_EMPTY_OBJECT_STRING "\"~~iow-empty-object~~\""

namespace wjrpc{

class outgoing_holder;

class incoming_holder
{
public:
  typedef std::function< void(outgoing_holder) > outgoing_handler_t;
  typedef std::function< void(incoming_holder, io_id_t, outgoing_handler_t )> incoming_handler_t;

  typedef ::wjson::json_error json_error;
  typedef data_type::iterator  iterator;
  typedef std::pair< iterator, iterator> raw_t;
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point;

public:

  incoming_holder();
  explicit incoming_holder(data_ptr    d,  time_point tp=time_point() );
  explicit incoming_holder(data_type   d,  time_point tp=time_point() );
  explicit incoming_holder(std::string d,  time_point tp=time_point() );

  data_ptr parse(::wjson::json_error* e);
  operator bool () const;
  bool ready() const;

  bool has_method() const;
  bool has_result() const;
  bool has_error() const;
  bool has_id() const;
  bool has_params() const;

  bool is_request() const;
  bool is_response() const;
  bool is_notify() const;
  bool is_error() const;
  bool is_request_error() const;
  bool is_other_error() const;

  bool is_valid() const;

  void attach(data_ptr d, time_point tp=time_point() );
  data_ptr detach();
  bool method_equal_to(const char* name) const;
  std::string method() const;
  raw_t raw_method() const;
  raw_t raw_id() const;

  template<typename V, typename J = ::wjson::value<V> >
  V get_id( ::wjson::json_error* e) const;

  template<typename J>
  std::unique_ptr<typename J::target> get_params( ::wjson::json_error* e) const;

  template<typename J>
  std::unique_ptr<typename J::target> get_result( ::wjson::json_error* e) const;

  template<typename J>
  std::unique_ptr<typename J::target> get_error( ::wjson::json_error* e) const;

  const incoming& get() const ;

  time_point get_time_point() const ;

  data_ptr acquire_params();

  incoming_holder clone() const;

  std::string str() const;

  std::string error_message(const ::wjson::json_error& e) const;

  std::string params_error_message(const ::wjson::json_error& e) const;

  std::string result_error_message(const ::wjson::json_error& e) const;

  std::string error_error_message(const ::wjson::json_error& e) const;

  std::string id_error_message(const ::wjson::json_error& e) const;

private:

  bool ready_() const{ return _data != nullptr && _parsed;}

private:

  bool     _parsed;
  data_ptr _data;
  incoming _incoming;
  iterator _begin;
  iterator _end;

  time_point _time_point;
};

/// /////////////////////////////////////
/// /////////////////////////////////////
/// /////////////////////////////////////

template<typename V, typename J /*= ::wjson::value<V>*/ >
V incoming_holder::get_id( ::wjson::json_error* e) const
{
  V id = V();
  if ( this->ready_() )
  {
    typename J::serializer()( id, _incoming.id.first, _incoming.id.second, e );
  }
  return id;
}

template<typename J>
std::unique_ptr<typename J::target> incoming_holder::get_result( ::wjson::json_error* e) const
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
std::unique_ptr<typename J::target> incoming_holder::get_params( ::wjson::json_error* e) const
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
std::unique_ptr<typename J::target> incoming_holder::get_error( ::wjson::json_error* e) const
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
