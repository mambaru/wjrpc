#include "calc/calc1.hpp"
#include "calc/calc_p.hpp"
#include "gateway.hpp"
#include "service.hpp"
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <chrono>

/*#ifndef NDEBUG
#define LOG_READ(BEG, END) std::clog << getpid() << " READ: " << std::string(BEG, END) << std::endl;
#define LOG_WRITE(BEG, END) std::clog << getpid() << " WRITE: " << std::string(BEG, END) << std::endl;
#else*/
#define LOG_READ(BEG, END)
#define LOG_WRITE(BEG, END)
/*#endif*/

wjrpc::io_id_t create_id()
{
  static std::atomic<wjrpc::io_id_t> counter( (wjrpc::io_id_t(1)) );
  return counter.fetch_add(1);
}

class calc_server
{
public:
  void initialize(std::shared_ptr<icalc> target);
  void run(int rd, int wd);
private:
  class engine;
  std::shared_ptr<engine> _impl;  
  std::shared_ptr<icalc>  _target;
};

class calc_server::engine: public service::engine_type {};

void calc_server::initialize(std::shared_ptr<icalc> target)
{
  _impl = std::make_shared<engine>();
  _target = target;
}

void calc_server::run(int rd, int wd)
{
  engine::options_type opt;
  opt.target = _target;
  _impl->start(opt, create_id() );

  auto io_id = create_id();  
  char buff[1024];
  for (;;)
  {
    int s = ::read( rd, buff, 1024 );
    LOG_READ(buff, buff + s)

    auto d = std::make_unique<wjrpc::data_type>( buff, buff + s );

    _impl->perform_io( std::move(d), io_id, [wd]( wjrpc::data_ptr d )
    {
      LOG_WRITE(d->begin(), d->end() )
      ::write( wd, d->data(), d->size());
    });
  }
}

class calc_client
{
public:
  void initialize(int rd, int wd);
  std::shared_ptr<icalc> get();
private:
  class engine;
  std::shared_ptr<engine> _impl;
  std::shared_ptr<icalc> _calc;
};

class calc_client::engine: public gateway::engine_type {};

void calc_client::initialize(int rd, int wd)
{
  _impl = std::make_shared<engine>();
  engine::options_type opt;
  _impl->start(opt, create_id() );  

  auto io_id = create_id();
  using namespace std::placeholders;
  _impl->reg_io( io_id,  [rd, wd]( wjrpc::data_ptr d, wjrpc::io_id_t, wjrpc::output_handler_t handler)
  {
    LOG_WRITE(d->begin(), d->end() )
    ::write( wd, d->data(), d->size() );
    char buff[1024];
    int s = ::read(rd, buff, 1024);
    LOG_READ(buff, buff + s)
    handler( std::make_unique<wjrpc::data_type>(buff, buff + s) );
  } );
  _calc = _impl->find(io_id);
}

std::shared_ptr<icalc> calc_client::get()
{
  return _calc;
}

void create_chain(int mode, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p> proxy);
std::shared_ptr<icalc> create_chain(int mode, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p> proxy)
{
  if ( count == 0)
    return calc;

  // mode - 0, прямой вызов
  // mode - 1, сериализация без пайпов
  // mode - 2, через пайпы
  if ( mode==0 )
  {
    if ( count == 0)
      return calc;
  }
  else if ( mode==1 )
  {
    
  }
  else if ( mode==2 )
  {
    int up[2], down[2];
    ::pipe(up);
    ::pipe(down);
    auto target = create_chain(mode, calc, count - 1, proxy);
    auto pid = fork();
    if ( pid == 0 )
    {
      // дочериний процесс
      auto srv = std::make_shared<calc_server>();
      srv->initialize(target);
      // бесконечный цикл
      srv->run(up[0], down[1]);
    }
    else
    {
      // текущий процесс
      // создаем шлюз 
      auto cli = std::make_shared<calc_client>();
      cli->initialize(down[0], up[1]);
       
    }
  }
  abort();
}



//*****//
//*****//
//*****//
//*****//
void run_service(int rd, int wd, std::shared_ptr<service::engine_type> srv)
{
  auto io_id = create_id();
  char buff[1024];
  for (;;)
  {
    int s = ::read( rd, buff, 1024 );
    LOG_READ(buff, buff + s)

    auto d = std::make_unique<wjrpc::data_type>( buff, buff + s );

    srv->perform_io( std::move(d), io_id, [wd]( wjrpc::data_ptr d )
    {
      LOG_WRITE(d->begin(), d->end() )
      ::write( wd, d->data(), d->size());
    });
  }
}

void gateway_writer(int rd, int wd, wjrpc::data_ptr d, wjrpc::output_handler_t handler)
{
  LOG_WRITE(d->begin(), d->end() )
  ::write( wd, d->data(), d->size() );
  char buff[1024];
  int s = ::read(rd, buff, 1024);
  LOG_READ(buff, buff + s)
  handler( std::make_unique<wjrpc::data_type>(buff, buff + s) );
}

void run_calc(int rd, int wd, std::shared_ptr<icalc> calc)
{
  auto srv = std::make_shared<service::engine_type>();
  service::engine_type::options_type srv_opt;
  srv_opt.target = calc;
  srv->start(srv_opt, 11);
  run_service(rd, wd, srv);
}

struct holder
{
  int rd = 0, wd = 0;
  std::shared_ptr<gateway::engine_type> engine;
  std::shared_ptr<icalc> calc;
};

void run_proxy(int rd, int wd, std::shared_ptr<icalc> calc, int count, std::shared_ptr<calc_p> proxy);

holder create_gateway(std::shared_ptr<icalc> calc, int count, std::shared_ptr<calc_p> proxy)
{
  if ( count == 0 )
    return holder();
  holder hld;
  int up[2], down[2];
  ::pipe(up);
  ::pipe(down);
  auto pid = fork();
  if ( pid == 0 )
  {
    run_proxy(up[0], down[1], calc, count-1 , proxy);
    return holder();
  }

  hld.rd = down[0];
  hld.wd = up[1];
  hld.engine = std::make_shared<gateway::engine_type>();
  gateway::engine_type::options_type gtw_opt;
  hld.engine->start(gtw_opt, 33);
  using namespace std::placeholders;
  hld.engine->reg_io( 44,  std::bind(gateway_writer, down[0], up[1], _1, _3) );
  hld.calc = hld.engine->find(44);
  return hld;
}

void run_proxy(int rd, int wd, std::shared_ptr<icalc> calc, int count, std::shared_ptr<calc_p> proxy)
{
  if ( count == 0 )
    return run_calc(rd, wd, calc);
  
  auto hld = create_gateway(calc, count, proxy);
  auto srv = std::make_shared<service::engine_type>();

  service::engine_type::options_type srv_opt;
  if ( proxy!=nullptr )
  {
    proxy->initialize( hld.calc );
    srv_opt.target = proxy;
  }
  else
  {
    srv_opt.target = hld.calc;
  }
  srv->start(srv_opt, 22);

  run_service(rd, wd, srv);
}

template<typename T>
T get(std::istream& is)
{
  T res = 0;
  is >> res;
  return res;
}

void run_iter(std::shared_ptr<icalc> cli)
{
  while ( !std::cin.eof() )
  {
    int first = get<int>(std::cin);
    char op = get<char>(std::cin);
    int second = get<int>(std::cin);
    if ( op == '+') 
    {
      auto req = std::make_unique<request::plus>();
      req->first = first;
      req->second = second;
      cli->plus(std::move(req), [](response::plus::ptr res)
      {
        std::cout << "= ";
        if (res) std::cout << res->value << std::endl;
        else std::cout << "null" << std::endl;
      });
    }
  }
}

void run_bench(std::shared_ptr<icalc> cli)
{
  std::vector<time_t> times;
  times.reserve(1000);
  request::plus pls; 
  pls.first = 12345;
  pls.second = 54321;
  for (;;)
  {
    auto start = std::chrono::high_resolution_clock::now();
    cli->plus( std::make_unique<request::plus>(pls), [](response::plus::ptr){} );
    auto finish = std::chrono::high_resolution_clock::now();
    auto span = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    times.push_back(span);
    if ( times.size() == 1000 )
    {
      std::sort(times.begin(), times.end());
      std::cout << "100%: " << times.back() << " ns ( " << size_t(( 1000000000.0/times.back() ) * 1) << " persec), "
                << "99%: " << times[990] << " ns ( " << size_t(( 1000000000.0/times[990] ) * 1) << " persec), "
                << "90%: " << times[900] << " ns ( " << size_t(( 1000000000.0/times[900] ) * 1) << " persec), "
                << "80%: " << times[800] << " ns ( " << size_t(( 1000000000.0/times[800] ) * 1) << " persec), "
                << "50%: " << times[500] << " ns ( " << size_t(( 1000000000.0/times[500] ) * 1) << " persec), " 
                << "0% " << times.front() << " ns ( " << size_t(( 1000000000.0/times.front() ) * 1) << " persec)" << std::endl;
      times.clear();
    }
  }
}


void run_client(int mode, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p> proxy)
{
  std::shared_ptr<icalc> clc = calc;
  auto hld = create_gateway(calc, count, proxy);
  if ( count != 0 )
    clc = hld.calc;
  if ( mode == 0)
    run_iter( clc );
  if ( mode == 1)
    run_bench( clc );
}

int main(int argc, char* argv[])
{
  std::shared_ptr<calc_p> proxy;
  std::shared_ptr<calc1>  calc = std::make_shared<calc1>();

  if ( argc < 4)
  {
    std::cout << "Usage: calc [i|b|s] [<<proxy count>>] [w]" << std::endl;
  }


  int mode = 0;
  int proxy_count = 0;

  if ( argc > 1 )
  {
    if ( argv[1][0] == 'i' ) mode = 0;
    else if ( argv[1][0] == 'b' ) mode = 1;
    else if ( argv[1][0] == 's' ) mode = 2;
  }

  if ( argc > 2 )
    proxy_count = atoi(argv[2]);

  if ( argc > 3 )
    proxy = std::make_shared<calc_p>();
  
  std::cout << "mode: " << mode << std::endl; 
  std::cout << "proxy_count: " << proxy_count << std::endl;
  std::cout << "proxy: " << std::boolalpha <<  ( proxy!=nullptr ) << std::endl;
  run_client(mode, calc, proxy_count, proxy);
  
  /*
  int mode1 = atoi(argv[1]);
  int mode2 = atoi(argv[2]);
  run(mode1, mode2);
  */
}


