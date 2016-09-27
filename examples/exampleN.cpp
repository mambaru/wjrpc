#include <wjrpc/handler.hpp>
#include <wjrpc/engine.hpp>
#include <wjrpc/method.hpp>

#include <algorithm>
#include <memory>

struct handler_types: wjrpc::handler_types 
{
  typedef unsigned short io_id_t;
};

struct method_list: wjrpc::method_list
<
  fas::value<wjrpc::_handler_types_, handler_types >
  /* список методов */
>
{};


/* Тип обработчик JSON-RPC запросов */
struct handler: wjrpc::handler<method_list> {};

typedef wjrpc::engine< handler > jsonrpc_engine;

int main()
{
  auto jrpc = std::make_shared<jsonrpc_engine>();
  jsonrpc_engine::options_type opt;
  jrpc->start(opt, -1000L);
  std::string sreq = "{\"method\":\"plus\",\"params\":{\"first\":2, \"second\":3},\"id\":1}";
  jrpc->perform_io( std::make_unique< wjrpc::data_type >(sreq.begin(), sreq.end()), -1000, []( wjrpc::data_ptr d) 
  {
    std::cout << "responce: " << std::string( d->begin(), d->end() ) << std::endl;
  });
}


