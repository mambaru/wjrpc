//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <sstream>
#include <functional>

namespace wjrpc {

struct ilogger
{
  virtual ~ilogger() {}
  virtual void log_error(const std::string& msg) const = 0;
  virtual void log_fatal(const std::string& msg) const = 0;
  virtual void log_debug(const std::string& msg) const = 0;
  virtual void log_trace(const std::string& msg) const = 0;
};

struct logger_options
{
  typedef std::function<void(const std::string& )> log_fun_t;
  log_fun_t log_error;
  log_fun_t log_fatal;
  log_fun_t log_debug;
  log_fun_t log_trace;
};

class logger
  : public ilogger
{
public:
  void initialize(const logger_options& opt)
  {
    _opt = opt;
  }

  logger_options get_opt() const 
  {
    return _opt;
  }

  virtual void log_error(const std::string& msg) const override 
  {
    if ( _opt.log_error!=nullptr )
      _opt.log_error(msg);
  }

  virtual void log_fatal(const std::string& msg) const  override 
  {
    if ( _opt.log_fatal!=nullptr )
      _opt.log_fatal(msg);
  }

  virtual void log_debug(const std::string& msg) const  override 
  {
    if ( _opt.log_debug!=nullptr )
      _opt.log_debug(msg);
  }

  virtual void log_trace(const std::string& msg) const override 
  {
    if ( _opt.log_trace!=nullptr )
      _opt.log_trace(msg);
  }
private:
  logger_options _opt;
};

}

#define WJRPC_LOG(T, M, X) { std::stringstream ss; ss << X; (T)->M(ss.str()); }

#define WJRPC_LOG_ERROR(T, X)    WJRPC_LOG( T, log_error, X )
#define WJRPC_LOG_FATAL(T, X)    WJRPC_LOG( T, log_fatal, X )
#define WJRPC_LOG_DEBUG(T, X)    WJRPC_LOG( T, log_debug, X )
#define WJRPC_LOG_TRACE(T, X)    WJRPC_LOG( T, log_trace, X )
