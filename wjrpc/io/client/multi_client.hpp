#pragma once

#include <iow/io/client/multi_base.hpp>
#include <iow/logger/logger.hpp>
#include <mutex>
#include <memory>
#include <vector>

namespace iow{ namespace io{ namespace client{

template<typename Client>
class multi_client: public multi_base<Client>
{
  typedef multi_base<Client> super;

public:
  typedef typename super::io_service_type io_service_type;
  
  multi_client(io_service_type& io)
    : super(io)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    super::start(std::move(opt), opt.connect_count);
  }

  template<typename Opt>
  void reconfigure(Opt opt, size_t count)
  {
    super::reconfigure(std::move(opt), opt.connect_count);
  }
};

}}}
