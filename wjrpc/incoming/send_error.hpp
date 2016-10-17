//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjrpc/types.hpp>

namespace wjrpc{ namespace aux{

void send_error( incoming_holder holder, std::unique_ptr<error>, outgoing_handler_t outgoing_handler);
void send_error_raw( incoming_holder holder, std::unique_ptr<error>, output_handler_t output_handler);

}}
