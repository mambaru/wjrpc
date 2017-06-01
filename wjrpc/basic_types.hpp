//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace wjrpc{

typedef size_t io_id_t;
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef data_ptr incoming_call_id_t;
typedef int      call_id_t;
typedef int      error_code_t;

typedef std::function< void(data_ptr) > output_handler_t;
typedef std::function< void(data_ptr, io_id_t, output_handler_t )> input_handler_t;


}

