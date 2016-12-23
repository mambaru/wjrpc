//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <wjrpc/basic_types.hpp>

namespace wjrpc{
  
enum class error_codes
{
  NoError = 0,
  ParseError=-32700, 
  InvalidRequest = -32600, 
  ProcedureNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603,
  ServerError = -32000,
  MethodNotImplementation = -32001,
  BadGateway = -32002,
  ServiceUnavailable = -32003,
  QueueOverflow = -32004,
};


struct error
{
  error_code_t code = 0;
  error():code() {}
  error(error_codes code)
    : code( static_cast<error_code_t>(code) )
  {}

  typedef std::unique_ptr<error> ptr;
};



struct parse_error: error
{
  parse_error()
    : error(error_codes::ParseError)
  {}
};

struct invalid_request: error
{
  invalid_request()
    : error(error_codes::InvalidRequest)
  {}
};

struct procedure_not_found: error
{
  procedure_not_found()
    : error(error_codes::ProcedureNotFound)
  {}
};

struct invalid_params: error
{
  invalid_params()
    : error(error_codes::InvalidParams)
  {}
};

struct internal_error: error
{
  internal_error()
    : error(error_codes::InternalError)
  {}
};

struct method_not_impl: error
{
  method_not_impl()
    : error(error_codes::MethodNotImplementation)
  {}
};

// сетевая ошибка
struct bad_gateway: error
{
  bad_gateway()
    : error(error_codes::BadGateway)
  {}
};

// ошибка конфигурации
struct service_unavailable: error
{
  service_unavailable()
    : error(error_codes::ServiceUnavailable)
  {}
};


struct server_error: error
{
  server_error()
    : error(error_codes::ServerError)
  {}
};

}
