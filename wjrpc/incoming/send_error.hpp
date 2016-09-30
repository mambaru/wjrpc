#pragma once

#include <wjrpc/types.hpp>

namespace wjrpc{ namespace aux{

void send_error( incoming_holder holder, std::unique_ptr<error>, outgoing_handler_t outgoing_handler);
void send_error_raw( incoming_holder holder, std::unique_ptr<error>, output_handler_t output_handler);

}}
