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

    /*
    try
    {
      t << message("bad parse: [") <<  r << "]";
      t << fatal("GOOD PARSE: [") << r << "]";
    }
    catch(const ::wjson::json_error& e)
    {
      t << message("parse error: ") <<  h.error_message(e);
    }
    */
  }

  /*
  t << message("-------------------------");
  for (auto r : bad_request )
  {
    incoming_holder h( std::make_unique<data_type>(r[0].begin(), r[0].end()) );
    t << message("bad request: ") <<  r[0];
    h.parse();
    t << is_true<assert>( h.has_params() ) << FAS_TESTING_FILE_LINE;
    try
    {
      h.get_params<noparams_json>();
      t << fatal("GOOD REQUEST");
    }
    catch(const iow::json::json_error& e)
    {
      t << message("parse error: ") <<  h.error_message(e);
    }
  }
  */

  t << nothing;
}

BEGIN_SUITE(incoming_suite, "")
  ADD_UNIT(incoming1)
END_SUITE(incoming_suite)