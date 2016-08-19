#include <iostream>
#include <wjrpc/method.hpp>
#include <wjrpc/noparams_json.hpp>

#include <fas/testing.hpp>

#include "req.hpp"

JSONRPC_TAG(method1)

#include <wjrpc/method/aspect/name.hpp>


UNIT(method1, "")
{
  using namespace fas::testing;
  using namespace ::wjrpc;

  t << nothing;
}

BEGIN_SUITE(method_suite, "")
  ADD_UNIT(method1)
END_SUITE(method_suite)