//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/errors/errors.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wjrpc{

struct error_codes_json
{
  JSON_NAME2(NoError, "No error")
  JSON_NAME2(ParseError, "Parse error")
  JSON_NAME2(InvalidRequest, "Invalid Request")
  JSON_NAME2(ProcedureNotFound, "Method not found")
  JSON_NAME2(InvalidParams, "Invalid params")
  JSON_NAME2(InternalError, "Internal error")
  JSON_NAME2(ServerError, "Server error")
  JSON_NAME2(MethodNotImplementation, "Method not implementation")
  JSON_NAME2(BadGateway, "Bad Gateway")
  JSON_NAME2(ServiceUnavailable, "Service unavailable")
  JSON_NAME2(QueueOverflow, "Queue overflow")

  typedef ::wjson::enumerator<
    error_code_t,
    ::wjson::member_list<
      ::wjson::enum_value<NoError, error_code_t, static_cast<error_code_t>(error_codes::NoError) >,
      ::wjson::enum_value<ParseError, error_code_t, static_cast<error_code_t>(error_codes::ParseError) >,
      ::wjson::enum_value<InvalidRequest, error_code_t, static_cast<error_code_t>(error_codes::InvalidRequest) >,
      ::wjson::enum_value<ProcedureNotFound, error_code_t, static_cast<error_code_t>(error_codes::ProcedureNotFound) >,
      ::wjson::enum_value<InvalidParams, error_code_t, static_cast<error_code_t>(error_codes::InvalidParams) >,
      ::wjson::enum_value<InternalError, error_code_t, static_cast<error_code_t>(error_codes::InternalError) >,
      ::wjson::enum_value<ServerError, error_code_t, static_cast<error_code_t>(error_codes::ServerError) >,
      ::wjson::enum_value<MethodNotImplementation, error_code_t, static_cast<error_code_t>(error_codes::MethodNotImplementation) >,
      ::wjson::enum_value<BadGateway, error_code_t, static_cast<error_code_t>(error_codes::BadGateway) >,
      ::wjson::enum_value<ServiceUnavailable, error_code_t, static_cast<error_code_t>(error_codes::ServiceUnavailable) >,
      ::wjson::enum_value<QueueOverflow, error_code_t, static_cast<error_code_t>(error_codes::QueueOverflow) >
    >
  > type;
  typedef type::target target; 
  typedef type::member_list member_list;
  typedef type::serializer  serializer;

};

  
struct error_json
{
  JSON_NAME(code)
  JSON_NAME(message)

  typedef ::wjson::object<
    error,
    ::wjson::member_list<
      ::wjson::member<n_code,    error, error_code_t,      &error::code>,
      ::wjson::member<n_message, error, error_code_t,      &error::code, error_codes_json>
    >
  > type;

  typedef error target; 
  typedef type::member_list member_list;
  typedef type::serializer  serializer;
};

}
