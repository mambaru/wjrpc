#pragma once
#include "calc/icalc.hpp"
#include <memory>

class pclient
{
public:
  void initialize(int rd, int wd);
  std::shared_ptr<icalc> get();
private:
  class engine;
  std::shared_ptr<engine> _impl;
  std::shared_ptr<icalc> _calc;
};
