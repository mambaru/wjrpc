//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2022
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <functional>
#include <memory>

namespace wjrpc{

/** 
 * Заглушка для пользовательской обертки функции обратного вызова  
*/
struct ad_callback_stub
{
  template<typename T, typename Opt>
  void initialize(T& , const Opt& )
  {
    
  }
 
  template<typename T, typename Result>
  std::function<void(std::unique_ptr<Result>)>
    operator()(T& , std::function<void(std::unique_ptr<Result>)> cb)
  {
    return cb;
  }
};

} // wjrpc



