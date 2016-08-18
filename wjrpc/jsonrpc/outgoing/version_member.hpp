#pragma once

#include <iow/jsonrpc/outgoing/outgoing.hpp>
#include <wjson/name.hpp>
#include <wjson/json.hpp>

namespace iow{ namespace jsonrpc{

struct version_member
{
  JSON_NAME(jsonrpc)
  typedef ::wjson::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}

