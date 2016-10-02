#include "gateway.hpp"
#include "service.hpp"
#include <iostream>

std::shared_ptr<icalc> create_next(int mode1)
{
  
}

std::shared_ptr<icalc> create_client(int mode1, std::shared_ptr<icalc> next)
{
  return nullptr;
}

void run(int mode1, int mode2)
{
  auto cli = create_client(mode1, create_next(mode2) );
  while ( !std::cin.eof() )
  {
    int first = 0;
    char op = '';
    int second = 0;
    std::cin >> first;
    std::cin >> op;
    std::cin >> second;
    
  }
}

int main(int argc, char* argv[])
{
  if ( argc < 3)
  {
    std::cout << "Usage: calc <<mode1>> <<mode2>>" << std::endl;
    std::cout << "\tmode1" << std::endl;
    std::cout << "\t\t0 - interactive" << std::endl;
    std::cout << "\t\t1 - benchmark" << std::endl;
    std::cout << "\t\t2 - stress" << std::endl;
    std::cout << "\tmode2" << std::endl;
    std::cout << "\t\t0 - local" << std::endl;
    std::cout << "\t\t1 - local->server" << std::endl;
    std::cout << "\t\t2 - local->proxy->server" << std::endl;
  }
  int mode1 = atoi(argv[1]);
  int mode2 = atoi(argv[2]);
  run(mode1, mode2);
}


