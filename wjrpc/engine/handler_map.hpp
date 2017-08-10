#pragma once

#include <fas/aop.hpp>
#include <mutex>
#include <memory>



namespace wjrpc{

template<typename Handler>
class handler_map
{
public:
  typedef Handler handler_type;
  typedef typename handler_type::ptr handler_ptr;
  typedef typename handler_type::io_id_t io_id_t;

  struct data
  {
    handler_ptr first;
    bool reinit;
  };
  
  handler_map()
  {
  }
  
  
  void disable(handler_ptr value)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->_default = value;
  }

  handler_ptr find(io_id_t io_id) const
  {
    std::lock_guard< mutex_type > lk(_mutex);
    if ( _default!=nullptr )
      return _default;
      
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
      return nullptr;
    return itr->second.first;
  }

  handler_ptr findocre(io_id_t io_id, bool reg_io, bool& reinit)
  {
    /*
    if ( _disable )
    {
      reinit = _reinit.exchange(false);
#warning инит может пройти позже первого вызова (думай млин)
#warning надо делать на уровне engine
      return _default;
    }*/
    std::lock_guard<mutex_type> lk(_mutex);
    
    if ( _default!=nullptr )
    {
      reinit = false;
      return _default->clone();
    }

    auto itr = _handlers.find(io_id);
    if ( itr != _handlers.end() )
    {
      reinit = itr->second.reinit;
      itr->second.reinit = reg_io;
      return itr->second.first;
    }

    reinit = true;
    auto handler = std::make_shared<handler_type>();
    _handlers[io_id] = data{handler, reg_io};
    return handler;
  }

  bool erase(io_id_t io_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
      return false;
    _handlers.erase(itr);
    return true;
  }

  void stop()
  {
    handler_ptr def;
    handler_map_t tmp_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      tmp_map.swap(_handlers);
      def = _default;
      _default = nullptr;
    }

    if ( def ) def->stop();
    for (auto& tmp : tmp_map)
    {
      if (tmp.second.first != nullptr ) 
      {
        tmp.second.first->stop();
      }
    }
  }
  
  size_t size() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _handlers.size();
  }

  /*mutex_type& mutex() const
  {
    return _mutex;
  }*/
private:
  typedef std::map<io_id_t, data > handler_map_t;
  typedef std::mutex mutex_type;
  handler_map_t _handlers;
  mutable mutex_type _mutex;
  handler_ptr _default;
  //std::atomic<bool> _disable;
  //std::atomic<bool> _reinit;
};

} // wfc

