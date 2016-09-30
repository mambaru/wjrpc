#include <wjrpc/incoming/send_error.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>
#include <wjrpc/errors/error_json.hpp>
#include <wjrpc/types.hpp>
#include <wjrpc/logger.hpp>

#include <wjson/strerror.hpp>

namespace wjrpc{ namespace aux{

void send_error( incoming_holder holder, std::unique_ptr<error> err, outgoing_handler_t outgoing_handler)
{
  typedef outgoing_error_json< error_json > message_json;
  outgoing_error<error> error_message;
    
  error_message.error = std::move(err);
  auto id_range = holder.raw_id();
  if ( id_range.first != id_range.second )
  {
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
  }

  auto d = holder.detach();
  if ( d == nullptr )
    d = std::make_unique<data_type>();
  d->clear();
  d->reserve(80);
  typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));
  
  //WJRPC_LOG_ERROR( "jsonrpc-broker: " << d )
  outgoing_holder out(std::move(d));
  outgoing_handler( std::move(out) );
}

void send_error_raw( incoming_holder holder, std::unique_ptr<error> err, output_handler_t output_handler)
{
  typedef outgoing_error_json< error_json > message_json;
  outgoing_error<error> error_message;
    
  error_message.error = std::move(err);
  auto id_range = holder.raw_id();
  if ( id_range.first != id_range.second )
  {
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
  }

  auto d = holder.detach();
  if ( d == nullptr )
    d = std::make_unique<data_type>();
  d->clear();
  d->reserve(80);
  typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));

  //WJRPC_LOG_ERROR( "jsonrpc-broker: " << d )
  output_handler( std::move(d) );
}


}}
