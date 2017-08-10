//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/errors/errors.hpp>
#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/types.hpp>
#include <memory>

namespace wjrpc{

struct handler_types
{  
  typedef ::wjrpc::data_type data_type;
  typedef ::wjrpc::data_ptr  data_ptr;

  typedef ::wjrpc::io_id_t io_id_t;
  typedef ::wjrpc::call_id_t call_id_t;

  typedef ::wjrpc::incoming_handler_t incoming_handler_t;
  typedef ::wjrpc::outgoing_handler_t outgoing_handler_t;
  
  typedef ::wjrpc::error error_type;
  typedef ::wjrpc::error_json error_json;
  typedef std::unique_ptr<error> error_ptr;

  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;
  typedef std::function< void(incoming_holder) > result_handler_t;

  typedef std::function< void(const char* name, notify_serializer_t, request_serializer_t, result_handler_t ) > sender_handler_t;
  
};

} // wjrpc


