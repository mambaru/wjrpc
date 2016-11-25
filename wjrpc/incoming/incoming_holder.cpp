//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/method/aspect/send_error.hpp>
#include <wjrpc/incoming/send_error.hpp>
#include <wjrpc/errors.hpp>
#include <wjrpc/types.hpp>
#include <wjrpc/logger.hpp>
#include <wjson/strerror.hpp>

namespace wjrpc{

incoming_holder::incoming_holder()
  : _parsed(false)
{}

incoming_holder::incoming_holder(data_ptr d, time_point tp)
  : _parsed(false)
  , _data(std::move(d))
  , _time_point(tp)
{
}

incoming_holder::incoming_holder(data_type   d,  time_point tp )
  : incoming_holder( std::make_unique<data_type>( std::move(d) ), tp )
{
  
}

incoming_holder::incoming_holder(std::string d,  time_point tp )
  : incoming_holder( std::make_unique<data_type>( d.begin(), d.end() ), tp )
{
  
}

void incoming_holder::attach(data_ptr d,  time_point tp)
{
  if ( _data==nullptr || _data->empty() )
    _data = std::move(d);
  else if ( d!=nullptr )
    std::move( d->begin(), d->end(), std::back_inserter(*_data) );

  _parsed = false;
  _time_point = tp;
}


data_ptr incoming_holder::parse(::wjson::json_error* e)
{
  if ( _data == nullptr )
    return nullptr;

  _begin = ::wjson::parser::parse_space(_data->begin(), _data->end(), nullptr);
  _end = incoming_json::serializer()( _incoming, _begin, _data->end(), e);
  _parsed = !e || !*e;

  auto next = ::wjson::parser::parse_space( _end, _data->end(), nullptr);
  while ( next!=_data->end() && *next=='\0' ) ++next;
  if ( next != _data->end() )
  {
     auto res =  std::make_unique<data_type>(next, _data->end());
    _data->resize( std::distance(_data->begin(), next) );
    return std::move(res);
  }
  return nullptr;
}

size_t incoming_holder::size() const
{
  if ( _data!=nullptr )
    return _data->size();
  return 0;
}

incoming_holder::operator bool () const{ return this->ready_();}
bool incoming_holder::ready() const { return this->ready_();}

bool incoming_holder::has_method() const { return ready_() && _incoming.method.first!=_incoming.method.second; }
bool incoming_holder::has_result() const { return ready_() && _incoming.result.first!=_incoming.result.second; }
bool incoming_holder::has_error() const  { return ready_() && _incoming.error.first!=_incoming.error.second;   }
bool incoming_holder::has_id() const     { return ready_() && _incoming.id.first!=_incoming.id.second;         }
bool incoming_holder::has_params() const { return ready_() && _incoming.params.first!=_incoming.params.second; }

bool incoming_holder::is_request() const       { return this->has_method() && this->has_id();   }
bool incoming_holder::is_response() const      { return this->has_result() && this->has_id();   }
bool incoming_holder::is_notify() const        { return this->has_method() && !this->has_id();  }
bool incoming_holder::is_error() const         { return this->has_error();                      }
bool incoming_holder::is_request_error() const { return this->has_error() && this->has_id() && 'n'!=*_incoming.params.first;}
bool incoming_holder::is_other_error() const   { return this->has_error() && !this->has_id();   }

bool incoming_holder::is_valid() const 
{
  return this->is_request()
      || this->is_response()
      || this->is_notify()
      || this->is_error();
}

bool incoming_holder::method_equal_to(const char* name)  const
{
  if ( !this->ready_() )
    return false;

  incoming::iterator beg = _incoming.method.first;
  incoming::iterator end = _incoming.method.second;
  if (beg++==end) return false;
  if (beg==end--) return false;
  for (; beg != end && *beg==*name; ++beg, ++name);
  return beg==end && *name=='\0';
}

std::string incoming_holder::method() const
{
  if ( this->ready_() && std::distance(_incoming.method.first, _incoming.method.second ) > 2 )
  {
    return std::string( _incoming.method.first+1, _incoming.method.second-1);
  }
  return std::string();
}

incoming_holder::raw_t incoming_holder::raw_method() const 
{
  return std::make_pair( _incoming.method.first, _incoming.method.second );
}

incoming_holder::raw_t incoming_holder::raw_id() const
{
  return std::make_pair( _incoming.id.first, _incoming.id.second );
}

std::string incoming_holder::str() const
{
  if ( _data != nullptr )
    return std::string(_data->begin(), _data->end());
  return std::string();
}


std::string incoming_holder::error_message(const ::wjson::json_error& e) const
{
  if ( _data != nullptr   )
    return ::wjson::strerror::message_trace( e, _data->begin(), _data->end() );
  return ::wjson::strerror::message(e);
}

std::string incoming_holder::params_error_message(const ::wjson::json_error& e) const
{
  return ::wjson::strerror::message_trace( e, _incoming.params.first, _incoming.params.second);
}

std::string incoming_holder::result_error_message(const ::wjson::json_error& e) const
{
  return ::wjson::strerror::message_trace( e, _incoming.result.first, _incoming.result.second);
}

std::string incoming_holder::error_error_message(const ::wjson::json_error& e) const
{
  return ::wjson::strerror::message_trace( e, _incoming.error.first, _incoming.error.second);
}

std::string incoming_holder::id_error_message(const ::wjson::json_error& e) const
{
  return ::wjson::strerror::message_trace( e, _incoming.id.first, _incoming.id.second);
}

const incoming& incoming_holder::get()  const 
{
  return _incoming;
}

incoming_holder::time_point incoming_holder::get_time_point() const 
{
  return _time_point;
}

data_ptr incoming_holder::detach()
{
  _incoming = incoming();
  return std::move(_data);
}

incoming_holder incoming_holder::clone() const
{
  return incoming_holder(*_data, _time_point);
}

data_ptr incoming_holder::acquire_params()
{
  if ( !this->ready_() )
    return nullptr;

  std::move( _incoming.params.first, _incoming.params.second, _data->begin() );
  _data->resize( std::distance(_incoming.params.first, _incoming.params.second) );
  _incoming = incoming();
  return std::move(_data);
}

}
