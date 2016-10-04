#include "pclient.hpp"
#include "gateway.hpp"
#include "types.hpp"
#include <wjrpc/memory.hpp>
#include <unistd.h>



class pclient::engine: public gateway::engine_type {};

void pclient::initialize(int rd, int wd)
{
  std::cout << "pclient::initialize: " << rd << " " << wd << std::endl;
  _impl = std::make_shared<engine>();
  engine::options_type opt;
  _impl->start(opt, create_id() );  

  auto io_id = create_id();
  using namespace std::placeholders;
  _impl->reg_io( io_id,  [rd, wd]( wjrpc::data_ptr d, wjrpc::io_id_t, wjrpc::output_handler_t handler)
  {
    std::cout << "REG io " << std::endl;
    LOG_WRITE(d->begin(), d->end() )
    ::write( wd, d->data(), d->size() );
    char buff[1024];
    int s = ::read(rd, buff, 1024);
    LOG_READ(buff, buff + s)
    handler( std::make_unique<wjrpc::data_type>(buff, buff + s) );
  } );
  _calc = _impl->find(io_id);
}

std::shared_ptr<icalc> pclient::get()
{
  return _calc;
}
