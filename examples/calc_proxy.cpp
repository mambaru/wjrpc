#include "calc/calc1.hpp"
#include "calc/calc_p.hpp"
#include "calc/pserver.hpp"
#include "calc/pclient.hpp"
#include "calc/types.hpp"
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <chrono>

std::shared_ptr<icalc> create_chain(int mode, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p>& proxy);
void fork_next(int rd, int wd, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p> proxy);
void run_iter(std::shared_ptr<icalc> cli);
void run_bench(std::shared_ptr<icalc> cli);
void run_client(int mode, std::shared_ptr<calc1>& calc, int count, std::shared_ptr<calc_p> proxy);

void fork_next(int rd, int wd, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p> proxy)
{
  auto srv = std::make_shared<pserver>();
  if ( count == 0 )
  {
    srv->initialize(rd, wd, calc);
    srv->run();
    return;
  }

  int up[2], down[2];
  ::pipe(up);
  ::pipe(down);

  auto cli = std::make_shared<pclient>();
  cli->initialize(down[0], up[1] );
  auto gtw = cli->get();

  if ( proxy == nullptr )
  {
    srv->initialize(rd, wd, gtw);
  }
  else
  {
    proxy->initialize(gtw);
    srv->initialize(rd, wd, proxy);
  }

  if ( 0!=fork() )
  {
    srv->run();
  }
  else
  {
    fork_next(up[0], down[1], calc, count - 1, proxy);
  }
}


std::shared_ptr<icalc> create_chain(int mode, std::shared_ptr<calc1> calc, int count, std::shared_ptr<calc_p>& proxy)
{
  if ( count == 0)
    return calc;

  // mode - 0, прямой вызов
  // mode - 1, сериализация без пайпов
  // mode - 2, через пайпы
  if ( mode==0 )
  {
    static std::list< std::shared_ptr<calc_p> > proxy_list;
    proxy_list.push_back(std::make_shared<calc_p>());
    proxy_list.front()->initialize(calc);
    for (int i=1; i < count; i++)
    {
      auto cur_proxy = std::make_shared<calc_p>();
      
    }
  }
  else if ( mode==1 )
  {
    
  }
  else if ( mode==2 )
  {
    int up[2], down[2];
    ::pipe(up);
    ::pipe(down);
    auto cli = std::make_shared<pclient>();
    // Так делать не надо
    static std::shared_ptr<pclient> main_client = cli;
    cli->initialize(down[0], up[1] );
    auto clc = cli->get();
    auto pid = fork();
    if ( pid != 0 )
      return clc;

    fork_next(up[0], down[1], calc, count - 1, proxy);
  }
  return nullptr;
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
    cli->plus( std::make_unique<request::plus>(pls), [&](response::plus::ptr)
    {
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
      
    } );
  }
}


void run_client(int mode, std::shared_ptr<calc1>& calc, int count, std::shared_ptr<calc_p> proxy)
{
  std::shared_ptr<icalc> clc = create_chain(2, calc, count, proxy);
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


