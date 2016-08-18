#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <fas/testing.hpp>
#include <algorithm>


UNIT(outgoing1_unit, "")
{
  using namespace fas::testing;
  t << nothing;
}




BEGIN_SUITE(outgoing_suite, "")
  ADD_UNIT(outgoing1_unit)
END_SUITE(outgoing_suite)