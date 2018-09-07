#pragma once
#include <wjrpc/logger.hpp>
#include <wjrpc/engine/call_map.hpp>
#include <wjrpc/engine/handler_map.hpp>
#include <wjrpc/engine/engine_options.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/outgoing/outgoing_error.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/types.hpp>
#include <wjrpc/incoming/send_error.hpp>
#include <wjson/strerror.hpp>

#include <type_traits>
#include <atomic>

namespace wjrpc{

/**
 * @brief JSONRPC движок. По сути, реестр jsonrpc-обработчиков, который позволяет вынести их в отдельный модуль и управлять временем жизни
 * @tparam JsonrpcHandler - JSONRPC обработчик, может быть реализован с помощью `wjrpc::handler`
 * @details 
  */
template<typename JsonrpcHandler>
class engine
  : public std::enable_shared_from_this< engine<JsonrpcHandler> >
{
public:
  typedef engine<JsonrpcHandler> self;
  typedef JsonrpcHandler handler_type;
  typedef typename handler_type::target_type target_type;
  typedef typename handler_type::ptr handler_ptr;
  typedef typename handler_type::options_type handler_options_type;
  typedef typename handler_type::result_handler_t result_handler_t;
  typedef typename handler_type::request_serializer_t request_serializer_t;
  typedef typename handler_type::notify_serializer_t notify_serializer_t;
  
  typedef engine_options<handler_options_type> options_type;
  
  typedef typename handler_type::io_id_t io_id_t;
  typedef typename handler_type::outgoing_handler_t outgoing_handler_t;
  typedef typename handler_type::data_ptr data_ptr;
  typedef typename handler_type::call_id_t call_id_t;
  

  ~engine()
  {
    this->stop();
  }
  
  engine()
    : _call_counter(1)
    , _log_time1(time(0))
    , _lost_results(0)
  {
  }

  /***************************************************************/
  /* Управляющие методы                                          */
  /***************************************************************/

  template<typename O>
  void start(O&& opt, io_id_t io_id)
  {
    _io_id = io_id;
    this->reconfigure( std::forward<O>(opt) );
  }

  template<typename O>
  void reconfigure(O&& opt1)
  {
    typename std::decay<O>::type opt = opt1;

    if ( opt.disable_handler_map )
    {
      auto handler = std::make_shared<handler_type>();
      handler->initialize(opt1);
      _handler_map.disable(handler);
    }
    else
      _handler_map.disable(nullptr);

    _call_map.set_lifetime( opt.call_lifetime_ms, opt.remove_everytime );
    _outgoing_factory = 
      [opt, this](io_id_t io_id, outgoing_handler_t handler, bool reg_io_flag) 
        -> handler_ptr
    {
      return this->create_handler_(io_id, opt, std::move(handler), reg_io_flag);
    };

    _input_factory = 
      [opt, this](io_id_t io_id, input_handler_t handler, bool reg_io_flag) 
        -> handler_ptr
    {
      return this->create_handler_(io_id, opt, std::move(handler), reg_io_flag);
    };
 
    _output_factory = [opt, this](io_id_t io_id, output_handler_t handler, bool reg_io_flag) -> handler_ptr
    {
      return this->create_handler_(io_id, opt, std::move(handler), reg_io_flag);
    };
  }

  void stop()
  {
    _handler_map.stop();
    _call_map.clear();
  }

  template<typename Tg, typename Req, typename ...Args>
  void call(Req req, io_id_t io_id, Args&&... args)
  {
    if ( handler_ptr h = _handler_map.find(io_id) )
    {
      h->template call<Tg>(std::move(req), std::forward<Args>(args)...);
    }
  }

  /***************************************************************/
  /* jsonrpc                                                     */
  /***************************************************************/

  void reg_jsonrpc( io_id_t io_id, outgoing_handler_t handler )
  {
    _outgoing_factory(io_id, std::move(handler), true);
  }

  void perform_jsonrpc(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
  {
    this->perform_incoming_( std::move(holder), io_id, std::move(handler) );
  }

  /***************************************************************/
  /* data_ptr                                                    */
  /***************************************************************/
  
  outgoing_handler_t io2rpc( output_handler_t handler )
  {
    if ( handler == nullptr )
      return nullptr;

    return [handler](outgoing_holder holder)
    {
      auto d = holder.detach();
      handler( std::move( d ) );
    };
  }

  void reg_io( io_id_t io_id, input_handler_t handler )
  {
    this->_input_factory(io_id, std::move(handler), true);
  }

  void unreg_io( io_id_t io_id )
  {
    _handler_map.erase(io_id);
  }

  
  void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) 
  {
    using namespace std::placeholders;
    while ( d != nullptr )
    {
      incoming_holder holder(std::move(d));
      ::wjson::json_error e;
      d = holder.parse(&e);
      if ( !e && holder )
      {
        this->perform_io_once_( std::move(holder), io_id, handler );
      }
      else
      {
        WJRPC_LOG_ERROR( "jsonrpc::engine: Parse error: " << holder.str() )
        aux::send_error_raw( std::move(holder), std::make_unique<parse_error>(), [handler](data_ptr err)
        {
          if ( err != nullptr)
            { WJRPC_LOG_ERROR( std::string(err->begin(), err->end()) ) }
          handler( std::move(err) );
        });
      }
    }
  }

  io_id_t get_id() const
  {
    return _io_id;
  }
  
  size_t remove_outdated()
  {
    return _call_map.remove_outdated();
  }

  handler_ptr find(io_id_t io_id) const
  {
    return _handler_map.find(io_id);
  }

  struct sizes_info
  {
    size_t handler_map = 0;
    size_t result_map = 0;
    size_t result_queue = 0;
  };
  
  sizes_info sizes() const
  {
    sizes_info si;
    auto cmi = _call_map.sizes();
    si.result_map = cmi.first;
    si.result_queue = cmi.second;
    si.handler_map = _handler_map.size();
    return si;
  }

private:
  
  void perform_io_once_(incoming_holder holder, io_id_t io_id, output_handler_t handler)
  {
    this->perform_incoming_( std::move(holder), io_id, [handler](outgoing_holder oholder)
    {
      handler(oholder.detach());
    });
  }

  void perform_incoming_(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
  {
    if ( holder.is_notify() || holder.is_request() )
    {
      this->perform_request_( std::move(holder), io_id, std::move(handler) );
    }
    else if (holder.is_response() || holder.is_error() )
    {
      this->perform_response_( std::move(holder), io_id, std::move(handler) );
    }
    else
    {
      WJRPC_LOG_ERROR("jsonrpc::engine: Invalid Request: " << holder.str() )
      aux::send_error( std::move(holder), std::make_unique<invalid_request>(), [handler](outgoing_holder oholder)
      {
        auto h2 = oholder.clone( oholder.call_id() );
        if ( auto d = h2.detach() )
        {
          WJRPC_LOG_ERROR(std::string(d->begin(), d->end()) )
        }
        handler( std::move(oholder) );
      });
    }
  }

  void perform_request_(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
  {
    if ( auto h = _outgoing_factory(io_id, std::cref(handler), false) )
    {
      h->invoke( std::move(holder), std::move(handler) );
    }
    else
    {
      WJRPC_LOG_FATAL( "_outgoing_factory==nullptr engine.hpp:267" )
    }
  }

  void perform_response_(incoming_holder holder, io_id_t /*io_id*/, outgoing_handler_t handler) 
  {
    ::wjson::json_error e;
    call_id_t call_id = holder.template get_id<call_id_t>(&e);

    if ( auto result_handler = _call_map.detach(call_id) )
    {
      result_handler(std::move(holder));
    }
    else if ( handler!=nullptr )
    {
      if ( !e )
      {  
        ++_lost_results;
        time_t now = time(0);
        if ( now > _log_time1 )
        {
          WJRPC_LOG_ERROR( "jsonrpc::engin incoming response with call_id=" << call_id << " not found. Total lost results " << _lost_results)
          _log_time1 = now;
        }
      }
      else
      {
        WJRPC_LOG_ERROR( "jsonrpc::engin incoming response with call_id=" << call_id << " id error. " << ::wjson::strerror::message_trace( e, holder.get().id.first, holder.get().id.second ) )
      }
      handler( outgoing_holder() );
    }
  }

  template<typename O>
  void upgrate_options_(O& opt, output_handler_t handler)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
    opt.sender_handler = [handler, wthis](const char* name, notify_serializer_t ns1, request_serializer_t rs1, result_handler_t rh1)
    {
      auto pthis = wthis.lock();
      if ( pthis==nullptr )
        return;
      
      if ( rs1!=nullptr )
      {
        auto call_id = pthis->_call_counter.fetch_add(1);
        pthis->_call_map.set(call_id, std::move(rh1));
        auto d = std::move(rs1(name, call_id));
        handler(std::move(d));
      }
      else if (ns1!=nullptr)
      {
        auto d = std::move(ns1(name));
        handler( std::move(d));
      }
      else
      {
        handler(nullptr);
      }
    };
  }
  
  template<typename O>
  void upgrate_options_(O& opt, input_handler_t handler)
  {
    io_id_t io_id = this->_io_id;
    std::weak_ptr<self> wthis = this->shared_from_this();
    opt.sender_handler = [handler, wthis, io_id](const char* name, notify_serializer_t ns1, request_serializer_t rs1, result_handler_t rh1)
    {
      auto pthis = wthis.lock();
      if ( pthis == nullptr )
        return;
      if ( rs1!=nullptr && rh1!=nullptr )
      {
        auto cid = pthis->_call_counter.fetch_add(1);
        
        pthis->_call_map.set(cid, std::move(rh1));
        auto d = std::move( rs1(name, cid) );
        
        handler( std::move(d), io_id, [wthis, handler, io_id, cid](data_ptr d2)
        {
          auto pthis2 = wthis.lock();
          if ( pthis2 == nullptr )
            return;

          // если данные не могут быть отправленны
          if ( d2==nullptr )
          {
            auto rh = pthis2->_call_map.detach(cid);
            if ( rh == nullptr )
              return;
            
            data_ptr ed = std::make_unique<data_type>();
            ed->reserve(50);
            outgoing_error<error> err;
            
            data_ptr pid = std::make_unique<data_type>();
            ::wjson::value<int>::serializer()( cid, std::back_inserter(*pid));
            err.error = std::make_unique<service_unavailable>();
            err.id = std::move(pid);
            
            outgoing_error_json<error_json>::serializer()(err, std::back_inserter(*ed));
            incoming_holder empty_holder(std::move(ed));
            rh( std::move(empty_holder) );
            return;
          }

          pthis2->perform_io( std::move(d2), io_id, [io_id, handler](data_ptr d3)
          {
            handler( std::move(d3), io_id, nullptr);
          });
        });
      }
      else if ( ns1!=nullptr)
      {
        auto d = std::move(ns1(name));
        handler( std::move(d), io_id, nullptr);
      }
    };
  }

  template<typename O>
  void upgrate_options_(O& opt, outgoing_handler_t handler)
  {
    
    if ( handler == nullptr )
    {
      opt.sender_handler = nullptr;
      return;
    }
    
    std::weak_ptr<self> wthis = this->shared_from_this();
    opt.sender_handler = [handler, wthis](const char* name, notify_serializer_t ns1, request_serializer_t rs1, result_handler_t rh1)
    {
      auto pthis = wthis.lock();
      if ( pthis==nullptr )
        return;

      call_id_t call_id = 0;
      outgoing_holder::result_handler_t rhw = nullptr;

      if ( rs1!=nullptr && rh1!=nullptr )
      {
        io_id_t io_id = pthis->_io_id;
        rhw = [wthis,  handler, io_id](incoming_holder holder)
        {
          auto pthis2 = wthis.lock();
          if ( pthis2==nullptr )
            return;

          if ( holder.is_response() || holder.is_error() )
          {
            if ( auto h = pthis2->_call_map.detach( holder.get_id<call_id_t>(nullptr) ) )
            {
              h(std::move(holder) );
            }
          }
          else
          {
            pthis2->perform_jsonrpc( std::move(holder), io_id, [handler](outgoing_holder oholder)
            {
              handler( std::move(oholder) ) ;
            });
          }
        }; // rh=[](){}
        call_id = pthis->_call_counter.fetch_add(1);
        pthis->_call_map.set(call_id, std::move(rh1) );
      }
      
      if ( ns1!=nullptr )
      {
        outgoing_holder holder(name, std::move(ns1), std::move(rs1), std::move(rhw), call_id );
        handler( std::move(holder) );
      }
      else
      {
        handler( std::move(outgoing_holder()) );
      }
    }; // opt.sender_handler
  }

  template<typename Opt, typename OutgoingHandler>
  handler_ptr create_handler_(io_id_t io_id, Opt opt, OutgoingHandler handler, bool reg_io_flag)
  {
    bool reinit;
    auto ph = _handler_map.findocre(io_id, reg_io_flag, reinit);
    if ( reinit )
    {
      // ph->initialize(opt);
      // специализация в зависимости от типа OutgoingHandler
      this->upgrate_options_(opt, std::move(handler));
      if ( !ph->status() )
      {
        ph->start(opt, io_id);
      }
      else if ( reinit )
      {
        ph->initialize(opt);
      }
    }
    return ph;
  }

private:

  std::function<handler_ptr(io_id_t, outgoing_handler_t, bool)> _outgoing_factory;
  std::function<handler_ptr(io_id_t, output_handler_t, bool)> _output_factory;
  std::function<handler_ptr(io_id_t, input_handler_t, bool)> _input_factory;

  typedef handler_map<handler_type> handler_map_t;
  handler_map_t _handler_map;
  call_map _call_map;
  std::atomic<int> _call_counter;
  io_id_t _io_id = 0;
  handler_ptr _default_handler;
  std::atomic<time_t> _log_time1;
  std::atomic<time_t> _lost_results;
};

}
