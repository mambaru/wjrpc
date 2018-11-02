#pragma once

#include <calc/icalc.hpp>
#include <memory>

class pserver
{
public:
  void initialize(int rd, int wd, const std::shared_ptr<icalc>& target);
  
  [[noreturn]]
  void run();
private:
  class engine;
  int _rd = -1;
  int _wd = -1;
  std::shared_ptr<engine> _impl;  
  std::shared_ptr<icalc>  _target;
};

