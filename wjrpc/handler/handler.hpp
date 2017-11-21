//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/handler/aspect/tags.hpp>
#include <wjrpc/errors.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/incoming/send_error.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
#include <wjrpc/types.hpp>
#include <wjrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <mutex>
#include <iostream>

namespace wjrpc{

template<typename MethodList>
class handler
  : public MethodList
  , public std::enable_shared_from_this< handler<MethodList> >
{
public:
  typedef handler<MethodList> self;
  typedef std::shared_ptr<self> ptr;
  typedef MethodList super;
  typedef typename super::interface_type interface_type;
  typedef typename super::target_type target_type;
  typedef typename super::peeper_type peeper_type;
  typedef typename super::context_type context_type;
  typedef typename super::options_type options_type;
  
  //typedef typename super::holder_type holder_type;
  typedef typename super::io_id_t io_id_t;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  typedef typename super::mutex_type mutex_type;
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

  peeper_type peeper() const
  {
    return this->get_aspect().template get<_peeper_>();
  }
  
  context_type& context()
  {
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  std::vector<std::string> get_methods() const
  {
    // TODO: в аспект
    return fas::for_each_group<_method_>(*this, f_get_methods() ).result;
  }

  void invoke(incoming_holder holder, outgoing_handler_t outgoing_handler) 
  {
    std::lock_guard< mutex_type > lk( _mutex );
    super::get_aspect().template get<_invoke_>()(*this, std::move(holder), std::move(outgoing_handler) );
  }
  
  bool status() const 
  {
    return _io_id!=0;
  }
  
  io_id_t get_id() const
  {
    return _io_id;
  }

  template<typename O>
  void start(O&& opt, io_id_t io_id)
  {
    std::lock_guard< mutex_type > lk( _mutex );
    _io_id = io_id;
    this->get_aspect().template get<_initialize_>()(*this, std::move(opt) );
    this->get_aspect().template get<_connect_>()(*this, io_id );
  }

  void stop()
  {
    std::lock_guard< mutex_type > lk( _mutex );
    this->get_aspect().template get<_disconnect_>()(*this, _io_id );
    //super::stop_(*this);
  }

  template<typename O>
  void initialize(O&& opt)
  {
    std::lock_guard< mutex_type > lk( _mutex );
    this->get_aspect().template get<_initialize_>()(*this, std::move(opt) );
  }

  void perform_io(data_ptr d, output_handler_t h) 
  {
    using namespace std::placeholders;

    while ( d != nullptr )
    {
      incoming_holder iholder(std::move(d));
      ::wjson::json_error e;
      d = iholder.parse(&e);
      if ( !e && iholder )
      {
        this->invoke( std::move(iholder), [h](outgoing_holder oholder)
        {
          h(oholder.detach());
        });
      }
      else
      {
        aux::send_error_raw( std::move(iholder), std::make_unique<parse_error>(), h );
      }
    }
  }
  
  void perform(std::string str, std::function<void(std::string)> h) 
  {
    this->perform_io( std::make_unique<data_type>( str.begin(), str.end() ), [h](data_ptr d)
    {
      h( std::string(d->begin(), d->end()) );
    });
  }
  
  std::shared_ptr<self> clone()
  {
    auto clon = std::make_shared<self>();
    static_cast<super&>(*clon) = static_cast<const super&>(*this);
    clon->_io_id = _io_id;
    return clon;
  }

private:
  struct f_get_methods
  {
    std::vector<std::string> result;
    template<typename T, typename Tg>
    void operator()(T& t, fas::tag<Tg> )
    {
      result.push_back( t.get_aspect().template get<Tg>().name() );
    }
  };

private:
  mutable mutex_type _mutex;
  io_id_t _io_id = 0;
};

} // wjrpc

