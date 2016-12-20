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
  typedef std::shared_ptr<handler_type> handler_ptr;
  typedef typename handler_type::io_id_t io_id_t;

  struct data
  {
    handler_ptr first;
    bool reinit;
  };

  handler_ptr find(io_id_t io_id) const
  {
    std::lock_guard< mutex_type > lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
      return nullptr;
    return itr->second.first;
  }

  handler_ptr findocre(io_id_t io_id, bool reg_io, bool& reinit)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr != _handlers.end() )
    {
      reinit = itr->second.reinit;
      itr->second.reinit = reg_io;
      return itr->second.first;
    }
    else if ( !reg_io )
    {
      reinit = (_default==nullptr);
      if ( reinit )
        _default = std::make_shared<handler_type>();
      return _default;
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
    handler_map_t tmp_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      tmp_map.swap(_handlers);
    }

    for (auto& tmp : tmp_map)
    {
      if (tmp.second.first != nullptr ) 
      {
        tmp.second.first->stop();
      }
    }
  }

private:
  typedef std::map<io_id_t, data > handler_map_t;
  typedef std::mutex mutex_type;
  handler_map_t _handlers;
  mutable mutex_type _mutex;
  handler_ptr _default;
};

} // wfc

