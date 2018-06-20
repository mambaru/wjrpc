//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#ifndef WJRPC_DISABLE_LOG

#include <wlog/logging.hpp>

#define WJRPC_LOG_ERROR(X)    WLOG_LOG_ERROR( "JSON-RPC", X )
#define WJRPC_LOG_WARNING(X)  WLOG_LOG_WARNING( "JSON-RPC", X )
#define WJRPC_LOG_MESSAGE(X)  WLOG_LOG_MESSAGE( "JSON-RPC", X )
#define WJRPC_LOG_FATAL(X)    WLOG_LOG_FATAL( "JSON-RPC", X )
#define WJRPC_LOG_BEGIN(X)    WLOG_LOG_BEGIN( "JSON-RPC", X )
#define WJRPC_LOG_END(X)      WLOG_LOG_END( "JSON-RPC", X )
#define WJRPC_LOG_DEBUG(X)    WLOG_LOG_DEBUG( "JSON-RPC", X )
#define WJRPC_LOG_TRACE(X)    WLOG_LOG_TRACE( "JSON-RPC", X )
#define WJRPC_LOG_PROGRESS(X) WLOG_LOG_PROGRESS( "JSON-RPC", X )

#else

#define WJRPC_LOG_ERROR(X)    
#define WJRPC_LOG_WARNING(X)  
#define WJRPC_LOG_MESSAGE(X)  
#define WJRPC_LOG_FATAL(X)    
#define WJRPC_LOG_BEGIN(X)    
#define WJRPC_LOG_END(X)      
#define WJRPC_LOG_DEBUG(X)    
#define WJRPC_LOG_TRACE(X)    
#define WJRPC_LOG_PROGRESS(X) 

#endif

