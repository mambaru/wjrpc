//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/basic_types.hpp>

namespace wjrpc{

typedef std::function< void(outgoing_holder) > outgoing_handler_t;
typedef std::function< void(incoming_holder, io_id_t, outgoing_handler_t )> incoming_handler_t;
typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;

}

