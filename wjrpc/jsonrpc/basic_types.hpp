#pragma once

#include <vector>
#include <memory>

namespace iow{ namespace jsonrpc{

typedef size_t io_id_t;
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef data_ptr incoming_call_id_t;
typedef int      call_id_t;
typedef int      error_code_t;

typedef std::function< void(data_ptr) > raw_outgoing_handler_t;
typedef std::function< void(data_ptr, io_id_t, raw_outgoing_handler_t )> raw_incoming_handler_t;


}}

