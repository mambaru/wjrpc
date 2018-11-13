#include "pclient.hpp"
#include "gateway.hpp"
#include "types.hpp"
#include <wjrpc/memory.hpp>
#include <unistd.h>



class pclient::engine: public gateway::engine_type {};

void pclient::initialize(int rd, int wd)
{
  _impl = std::make_shared<engine>();
  engine::options_type opt;
  _impl->start(opt, create_id() );  

  auto io_id = create_id();
  using namespace std::placeholders;
  _impl->reg_io( io_id,  [rd, wd]( wjrpc::data_ptr d, wjrpc::io_id_t, wjrpc::output_handler_t handler)
  {
    LOG_WRITE(d->begin(), d->end() )
    if ( -1 == ::write( wd, d->data(), d->size() ) )
      return;
    char buff[1024];
    ssize_t s = ::read(rd, buff, 1024l);
    LOG_READ(buff, buff + s)
    handler( std::make_unique<wjrpc::data_type>(buff, buff + s) );
  } );
  _calc = _impl->find(io_id);
}

std::shared_ptr<icalc> pclient::get() const
{
  return _calc;
}
