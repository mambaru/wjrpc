#pragma once

#include <iow/io/stream/stream_options.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/reader/stream/tags.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/io/descriptor/stream/tags.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/system.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename ID, typename DataType, typename DataPtr>
struct context
{
  typedef ID io_id_t;
  typedef DataType data_type;
  typedef DataPtr  data_ptr;
  
  typedef std::function< void(data_ptr) > outgoing_handler_t;
  typedef std::function< void(data_ptr, io_id_t, outgoing_handler_t) > incoming_handler_t;
  typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
  typedef std::function< void(io_id_t) > shutdown_handler_t;

  outgoing_handler_t outgoing_handler;
  incoming_handler_t incoming_handler;
  startup_handler_t  startup_handler;
  shutdown_handler_t shutdown_handler;
};
  

namespace stream{

typedef ::iow::io::descriptor::context<
  size_t, 
  std::vector<char>, 
  std::unique_ptr<std::vector<char> >
> stream_context;

  
struct context: stream_context
  // --- ::basic::context<ID>
  // typedef ID io_id_t
{
  typedef ::iow::io::data_pool< stream_context::data_type> pool_type;
  typedef std::shared_ptr< pool_type> pool_ptr;

  /*
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type>  data_ptr;
  typedef ::iow::io::data_pool<data_type> pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;
  typedef size_t io_id_t;
  typedef std::function< void(data_ptr) > outgoing_handler_fun;
  typedef std::function< void(data_ptr, io_id_t, outgoing_handler_fun) > incoming_handler_fun;
  typedef std::function< void(io_id_t, outgoing_handler_fun) > startup_handler_fun;
  typedef std::function< void(io_id_t) > shutdown_handler_fun;

  outgoing_handler_fun outgoing_handler;
  incoming_handler_fun incoming_handler;
  startup_handler_fun  startup_handler;
  shutdown_handler_fun shutdown_handler;
  */
};

struct options: 
  public ::iow::io::stream::options<context::data_type>
{
  typedef context::data_type data_type;
  typedef context::data_ptr  data_ptr; 
  typedef context::pool_ptr              pool_ptr;
  typedef context::outgoing_handler_t  outgoing_handler_t;
  typedef context::incoming_handler_t  incoming_handler_t;
  typedef context::startup_handler_t   startup_handler_t;
  typedef context::shutdown_handler_t  shutdown_handler_t;

  outgoing_handler_t outgoing_handler;
  incoming_handler_t incoming_handler;
  startup_handler_t  startup_handler;
  shutdown_handler_t shutdown_handler;
  pool_ptr pool;
};

// Универсальный для всех
struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O&& opt) 
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    context_type& cntx = t.get_aspect().template get<_context_>();
    if (  opt.incoming_handler != nullptr )
    {
      std::weak_ptr<T> wthis = t.shared_from_this();
      auto callback = cntx.outgoing_handler;
      cntx.outgoing_handler = t.wrap([wthis, callback](typename context_type::data_ptr d)
      {
        if ( auto pthis = wthis.lock() )
        {
          std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
          if ( callback != nullptr )
          {
            callback(std::move(d));
          }
          else
          {
            pthis->get_aspect().template get<_incoming_>()( *pthis, std::move(d) );
          }
        }
      });
    }

    
    cntx.incoming_handler  = opt.incoming_handler;
    cntx.startup_handler   = opt.startup_handler;
    cntx.shutdown_handler  = opt.shutdown_handler;
    
    t.get_aspect().template get< ::iow::io::stream::_initialize_>()( t, std::forward<O>(opt) );
  }
};

struct ad_after_start
{
  // TODO: startup_handler
};

struct ad_before_stop
{
  // TODO: shutdown_handler
};


// Это descriptor::text, у descriptor::jsonrpc хандлеры другие будут
// А также хадлеры разные для dgram и stream
struct ad_incoming_handler
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    const auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.incoming_handler != nullptr )
    {
      cntx.incoming_handler(std::move(d), t.get_id_(t), cntx.outgoing_handler);
    }
    else
    {
      t.get_aspect().template get<_incoming_>()( t, std::move(d) );
    }
  }
};
  

struct ad_async_read_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H handler)
  {
    t.descriptor().async_read_some( ::iow::asio::buffer( p.first, p.second ), std::move(handler) );
  }
};

struct ad_async_write_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H handler)
  {
    t.descriptor().async_write_some( ::boost::asio::buffer( p.first, p.second ), handler);
  }
};

  
struct aspect: fas::aspect<
  
  fas::value< _context_, context >,
  fas::advice< ::iow::io::reader::stream::_incoming_, ad_incoming_handler>,
  fas::alias< _incoming_, ::iow::io::writer::_output_>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_read_some>,
  fas::advice< ::iow::io::writer::_write_some_, ad_async_write_some>,
  fas::advice< _initialize_, ad_initialize>,
  fas::group< ::iow::io::_initialize_, _initialize_>
  
>{};
  
}}}}
