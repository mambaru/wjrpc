//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/incoming/incoming_holder.hpp>

#include <queue>
#include <mutex>
#include <chrono>


namespace wjrpc{

class call_map
{
public:
  typedef std::function<void(incoming_holder holder)> result_handler_t;
  typedef std::map<call_id_t, result_handler_t> result_map;
  typedef std::mutex mutex_type;

  typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;
  typedef std::pair<time_point_t, call_id_t> time_pair;
  typedef std::priority_queue< time_pair > time_queue;
  typedef std::deque<call_id_t> call_list;
  
  void set_lifetime(time_t lifetime_ms, bool everytime);

  void set(call_id_t call_id, result_handler_t result);
  
  result_handler_t detach(call_id_t call_id);
  
  void clear();
  
  size_t remove_outdated();

   std::pair<size_t, size_t> sizes() const;

private:

  call_list get_call_list();

private:
  bool _everytime = true;
  time_t _lifetime_ms = 1000;
  time_queue _time_queue;
  result_map _result_map;
  mutable mutex_type _mutex;
};

} // wfc

