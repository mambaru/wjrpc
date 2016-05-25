#pragma once

#include <fas/aop.hpp>

#include <iow/logger/logger.hpp>
#include <iow/io/client/connection.hpp>
#include <iow/workflow/workflow.hpp>

#include <memory>
#include <cassert>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace client{

template<typename Connection >
class client 
  : public Connection
  , public std::enable_shared_from_this< client<Connection> >
{
public:
  typedef Connection super;
  typedef client<Connection> self;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::descriptor_type descriptor_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::outgoing_handler_type outgoing_handler_t;
  typedef std::vector< data_ptr > wait_data_t;
  
  client( io_service_type& io)
    : super(std::move(descriptor_type(io)) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
  {
    _workflow = std::make_shared< ::iow::workflow >(io, ::iow::queue_options() );
  }
  
  client( io_service_type& io, descriptor_type&& desc)
    : super(std::move(desc) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
  {
    _workflow = std::make_shared< ::iow::workflow >(io, ::iow::queue_options() );
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    IOW_LOG_DEBUG("Client connect start " << opt.addr << ":" << opt.port << "" )
    if ( _started ) return;
    if ( opt.args.workflow != nullptr )
    {
        _workflow = opt.args.workflow;
    }
    else
    {
      IOW_LOG_WARNING("iow::io::client workflow not set")
    }
    _started = true;
    _ready_for_write = false;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;

    this->initialize_(opt);
    super::connect_( *this, opt );
  }

  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::connect_( *this, std::forward<Opt>(opt) );
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    if ( !_started ) 
      return;
    _started = false;
    _ready_for_write = false;
    super::stop_(*this);
  }

  template<typename Handler>
  void shutdown(Handler&& handler)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::shutdown_( *this, std::forward<Handler>(handler) );
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    _ready_for_write = false;
    super::close_(*this);
  }

  data_ptr send(data_ptr d)
  {
    IOW_LOG_DEBUG("-5- client::send -1-")
    std::lock_guard<mutex_type> lk( super::mutex() );
    IOW_LOG_DEBUG("-5- client::send -2- " << d)
    
    if ( d==nullptr )
      return nullptr;    

    if ( _ready_for_write && _outgoing_handler!=nullptr )
    {
      IOW_LOG_DEBUG("-5- client::send -3- " << d)
      _outgoing_handler( std::move(d) );
      IOW_LOG_DEBUG("-5- client::send -3.1- " )
    }
    else
    {
      IOW_LOG_ERROR("Client not conected. Not send: [" << d << "]")
      return std::move(d);
    }
    return nullptr;
  }
  
  void send( data_ptr d, io_id_t , outgoing_handler_t handler)
  {
    auto dd = this->send( std::move(d) ) ;
    if ( dd!=nullptr && handler!=nullptr )
    {
      handler(nullptr);
    }
  }

private:
  
  template<typename Opt>
  void client_start_(Opt&& opt)
  {
    IOW_LOG_DEBUG("-8- client_start_ ")
    super::start_(*this, opt.connection);
    IOW_LOG_DEBUG("-8.1- client_start_ ")
  }
  
  void startup_handler_(io_id_t, outgoing_handler_t handler)
  {
    IOW_LOG_DEBUG( "-X- client::startup_handler_" )
    _ready_for_write = true;
    auto tmp = [handler](data_ptr d)
    {
      IOW_LOG_DEBUG( "-X1- TMP cleint::startup_handler_ " << d )
      handler(std::move(d));
      IOW_LOG_DEBUG( "-X2- TMP cleint::startup_handler_ " << d )
    };
    _outgoing_handler = tmp;
  }
  
  template<typename OptPtr>
  void delayed_reconnect_(OptPtr popt)
  {
    _workflow->delayed_post( 
      std::chrono::milliseconds( this->_reconnect_timeout_ms ),
      [popt, this](){
        this->connect(*popt);
      }
    );
  }

  template<typename Opt>
  void initialize_(Opt& opt)
  {
    auto popt = std::make_shared<Opt>(opt);
    auto startup_handler  = popt->connection.startup_handler;
    auto shutdown_handler = popt->connection.shutdown_handler;
    auto connect_handler  = popt->args.connect_handler;
    auto error_handler    = popt->args.error_handler;

    std::weak_ptr<self> wthis = this->shared_from_this();
    popt->args.connect_handler = [wthis, connect_handler, popt]()
    {
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        IOW_LOG_DEBUG("-7- iow::client::initialize_-> client_start_ ")
        pthis->client_start_(*popt);
        IOW_LOG_DEBUG("-7.1- iow::client::initialize_-> client_start_ ")
      }
    };
    
    popt->args.error_handler = [wthis, error_handler, popt](::iow::system::error_code ec)
    {
      if ( error_handler!=nullptr ) error_handler(ec);
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->delayed_reconnect_(popt);
      }
    };
    
    popt->connection.shutdown_handler = [wthis, shutdown_handler, popt]( io_id_t io_id) 
    {
      if ( shutdown_handler!=nullptr ) shutdown_handler(io_id);
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_ready_for_write = false;
        pthis->delayed_reconnect_(popt);
      }
    };

    popt->connection.startup_handler = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->startup_handler_(io_id, outgoing);
      }

      if ( startup_handler != nullptr )
      {
        startup_handler( io_id, outgoing);
      }
    };
      
    if ( popt->connection.incoming_handler == nullptr )
    {
      popt->connection.incoming_handler
        = [wthis]( data_ptr d, io_id_t /*io_id*/, outgoing_handler_t /*outgoing*/)
      {
        IOW_LOG_ERROR("Client incoming_handler not set [" << d << "]" )
      }; 
    }

    opt = *popt;
  }
private:
  bool _started;
  bool _ready_for_write;
  time_t _reconnect_timeout_ms;
  outgoing_handler_t _outgoing_handler;
  std::shared_ptr< ::iow::workflow > _workflow;
};

}}}
