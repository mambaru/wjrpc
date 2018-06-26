#include <iostream>
#include <wjrpc/method.hpp>
#include <wjrpc/noparams_json.hpp>
#include <fas/testing.hpp>
#include "req.hpp"

UNIT(incoming1, "")
{
  using namespace fas::testing;
  using namespace ::wjrpc;
  //std::string data = "{\"method\":\"test\"}";
  t << is_true<expect>(true) ;
  for (auto r : good_parse )
  {
    incoming_holder h( std::make_unique<data_type>(r.begin(), r.end()) );
    t << message("good parse: ") <<  r;
    t << flush;
    h.parse(nullptr);
  }
  
  for (auto r : bad_parse )
  {
    t << message("bad parse: [") <<  r << "]";
    incoming_holder h( std::make_unique<data_type>(r.begin(), r.end()) );
    ::wjson::json_error e;
    h.parse(&e);
    t << is_true<expect>( e ) << FAS_FL;
    t << message("JSON-RPC error: ") << ::wjson::strerror::message_trace(e, r.begin(), r.end() );
  }

  t << nothing;
}

UNIT(incoming2, "")
{
  using namespace fas::testing;
  using namespace ::wjrpc;
  using namespace std::chrono;
  using namespace wjson::literals;
  std::string json = "{'method':'test','params':[1,2,3]}"_json;
  incoming_holder h( std::make_unique<data_type>(json.begin(), json.end()), high_resolution_clock::now() );
  ::wjson::json_error e;
  h.parse(&e);
  auto errstr = h.error_error_message(e);
  auto tp = h.get_time_point();
  t << message(errstr) << " " << duration_cast< nanoseconds >( high_resolution_clock::now() - tp ).count();
  t << message( h.id_error_message(e) );
  t << message( h.result_error_message(e) );
  t << message( h.params_error_message(e) );
  
  t << message( std::string( h.raw_method().first,  h.raw_method().second ) );
  t << is_true<expect>( h.ready() ) << FAS_FL;
  t << is_true<expect>( h.is_valid() ) << FAS_FL;
  t << is_false<expect>( h.is_request_error() ) << FAS_FL;
  t << is_false<expect>( h.is_other_error() ) << FAS_FL;
  t << is_false<assert>( e ) << FAS_FL;
  t << stop;
  auto d = h.acquire_params();
  t << equal<expect, std::string>( "[1,2,3]", std::string(d->begin(), d->end() ) ) << FAS_FL;
  
  
}

BEGIN_SUITE(incoming_suite, "")
  ADD_UNIT(incoming1)
  ADD_UNIT(incoming2)
END_SUITE(incoming_suite)
