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
    /*if ( _lifetime_ms == 0 )
      time_queue().swap(_time_queue);*/
  }
  

  void call_map::set(call_id_t call_id, result_handler_t result)
  {
    if ( _everytime )
      this->remove_outdated();
    std::lock_guard<mutex_type> lk(_mutex);

    auto death_time = std::chrono::system_clock::now() + std::chrono::milliseconds(_lifetime_ms);
    _result_map.insert( 
      std::make_pair(
        call_id, 
        std::make_pair(death_time, result)
      ) 
    );
    _time_set.insert(std::make_pair(death_time, call_id));
    /*
    _result_map[call_id] = result;
    if ( _lifetime_ms != 0 )
    {
      _time_queue.push( std::make_pair( std::chrono::system_clock::now() + std::chrono::milliseconds(_lifetime_ms), call_id) );
    }
    */
  }
  
  call_map::result_handler_t call_map::detach(call_id_t call_id)
  {
    //if ( _everytime ) this->remove_outdated();
    std::lock_guard<mutex_type> lk(_mutex);
    result_handler_t result = nullptr;
    auto itr = _result_map.find(call_id);
    if ( itr!=_result_map.end() )
    {
      result = std::move( itr->second.second );
      _time_set.erase( std::make_pair( itr->second.first, itr->first)  );
      _result_map.erase(itr);
    }
    return std::move(result);
  }
  
  void call_map::clear()
  {
    result_map clear_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _result_map.swap(clear_map);
    }
    
    for (auto& tmp : clear_map)
    {
      if ( auto handler = tmp.second.second ) 
      {
        handler( incoming_holder( data_ptr() ) );
      }
    }
  }
  
  size_t call_map::remove_outdated()
  {
    auto handlers = this->detach_outdated_();
    for ( auto h : handlers ) 
      h( incoming_holder( data_ptr() ) );
    return handlers.size();
    /*
    size_t count = 0;
    auto calls = this->detach_outdated();
    for ( auto call_id : calls ) 
    {
      
      if ( auto handler = this->detach(call_id) )
      {
        handler( incoming_holder( data_ptr() ) );
        ++count;
      }
      
    }
    return count;
    */
  }

   std::pair<size_t, size_t> call_map::sizes() const
   {
     std::lock_guard<mutex_type> lk(_mutex);
     return std::make_pair( _result_map.size(), _time_set.size() );
   }

  call_map::outdated_list call_map::detach_outdated_()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    outdated_list odl;
    
    auto now = std::chrono::system_clock::now();
    while ( !_time_set.empty() && ( _time_set.begin()->first < now)  )
    {
      auto itr = _result_map.find( _time_set.begin()->second );
      if ( itr!=_result_map.end() && itr->second.second != nullptr )
        odl.push_back( std::move(itr->second.second) );
      _time_set.erase( _time_set.begin() );
      _result_map.erase(itr);
    }
    return std::move( odl );
    /*
    auto now = std::chrono::system_clock::now();
    while ( !_time_queue.empty() && ( _time_queue.front().first < now)  )
    {
      std::cout << _time_queue.size() << " " << calls.size() << " " 
                << std::chrono::duration_cast<std::chrono::milliseconds>(_time_queue.front().first - now).count() << std::endl;
      calls.push_back( std::move(_time_queue.front().second) );
      _time_queue.pop();
      
    }
    return std::move( calls );
    */
  }

} // wfc

