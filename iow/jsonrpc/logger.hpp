//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <sstream>

#define JSONRPC_LOG(T, M, X) { std::stringstream ss; ss << X; (T)->M(ss.str()); }

#define JSONRPC_LOG_ERROR(T, X)    JSONRPC_LOG( T, log_error, X )
#define JSONRPC_LOG_WARNING(T, X)
#define JSONRPC_LOG_MESSAGE(T, X)
#define JSONRPC_LOG_FATAL(T, X)    
#define JSONRPC_LOG_BEGIN(T, X)    
#define JSONRPC_LOG_END(T, X)      
#define JSONRPC_LOG_DEBUG(T, X)    
#define JSONRPC_LOG_TRACE(T, X)    
#define JSONRPC_LOG_PROGRESS(T, X) 

