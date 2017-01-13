//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/engine/call_map.hpp>
#include <wjrpc/logger.hpp>

#include <queue>
#include <mutex>
#include <chrono>
#include <iostream>


namespace wjrpc{

  void call_map::set_lifetime(time_t lifetime_ms, bool everytime) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _everytime = everytime;
    _lifetime_ms = lifetime_ms;
    if ( _lifetime_ms == 0 )
      time_queue().swap(_time_queue);
  }
  

  void call_map::set(call_id_t call_id, result_handler_t result)
  {
    if ( _everytime ) this->remove_outdated();
    std::lock_guard<mutex_type> lk(_mutex);
    _result_map[call_id] = result;
    if ( _lifetime_ms != 0 )
    {
      _time_queue.emplace( std::chrono::system_clock::now() + std::chrono::milliseconds(_lifetime_ms), call_id);
      //_time_queue.emplace( this->now_ms() + _lifetime_ms, call_id);
    }
  }
  
  call_map::result_handler_t call_map::detach(call_id_t call_id)
  {
    //if ( _everytime ) this->remove_outdated();
    std::lock_guard<mutex_type> lk(_mutex);
    result_handler_t result = nullptr;
    auto itr = _result_map.find(call_id);
    if ( itr!=_result_map.end() )
    {
      result = std::move( itr->second );
      _result_map.erase(itr);
    }
    return std::move(result);
  }
  
  void call_map::clear()
  {
    result_map tmp_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _result_map.swap(tmp_map);
    }
    
    for (auto& tmp : tmp_map)
    {
      if (tmp.second != nullptr ) 
      {
        tmp.second( incoming_holder( data_ptr() ) );
      }
    }
  }
  
  size_t call_map::remove_outdated()
  {
    size_t count = 0;
    auto calls = this->get_call_list();
    for ( auto call_id : calls ) 
    {
      if ( auto handler = this->detach(call_id) )
      {
        handler( incoming_holder( data_ptr() ) );
        ++count;
      }
    }
    return count;
  }

   std::pair<size_t, size_t> call_map::sizes() const
   {
     std::lock_guard<mutex_type> lk(_mutex);
     return std::make_pair( _result_map.size(), _time_queue.size() );
   }

  call_map::call_list call_map::get_call_list()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    call_list calls;
    //auto now = this->now_ms();
    auto now = std::chrono::system_clock::now();
    /*if ( !_time_queue.empty() && _time_queue.top().first < now )
      std::cout << _time_queue.size() << " " <<  _time_queue.top().first << " " << now << std::endl;
    else
      std::cout << "_time_queue.size() == " <<  _time_queue.size() << " " << now << std::endl;
    */
    std::cout << std::endl <<  "DEBUG _time_queue.size() = " << _time_queue.size()  << std::endl;
    while ( !_time_queue.empty() && ( _time_queue.top().first < now) /*< now*/ )
    {
      std::cout << _time_queue.size() << " " << calls.size() << " " 
                << std::chrono::duration_cast<std::chrono::milliseconds>(_time_queue.top().first - now).count() << std::endl;
      calls.push_back( std::move(_time_queue.top().second) );
      _time_queue.pop();
      
    }
    return std::move( calls );
  }

} // wfc

