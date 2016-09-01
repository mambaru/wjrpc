//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <sstream>
#include <functional>
#include <fas/aop.hpp>

namespace wjrpc {

typedef std::function<void(const std::string& )> log_fun_t;

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
  log_fun_t log_error;
  log_fun_t log_fatal;
  log_fun_t log_debug;
  log_fun_t log_trace;
};

struct _init_log_;
struct _log_error_;
struct _log_fatal_;
struct _log_debug_;
struct _log_trace_;

struct ad_init_log
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    t.get_aspect().template get<_log_error_>() = opt.log_error;
    t.get_aspect().template get<_log_fatal_>() = opt.log_fatal;
    t.get_aspect().template get<_log_debug_>() = opt.log_debug;
    t.get_aspect().template get<_log_debug_>() = opt.log_debug;
  }
};

typedef fas::type_list_n<
  fas::advice<_init_log_, ad_init_log>,
  fas::value<_log_error_, log_fun_t>,
  fas::value<_log_fatal_, log_fun_t>,
  fas::value<_log_debug_, log_fun_t>,
  fas::value<_log_trace_, log_fun_t>
>::type log_advice_list;

struct log_aspect: fas::aspect< log_advice_list > {};

class logger
  : public ilogger
  , public fas::aspect_class< log_aspect >
{
public:
  void initialize(const logger_options& opt)
  {
    this->get_aspect().get<_init_log_>()(*this, opt);
  }

  logger_options get_opt() const 
  {
    return _opt;
  }

  virtual void log_error(const std::string& msg) const override 
  {
    this->get_aspect().get<_log_error_>()(msg);
  }

  virtual void log_fatal(const std::string& msg) const  override 
  {
    this->get_aspect().get<_log_fatal_>()(msg);
  }

  virtual void log_debug(const std::string& msg) const  override 
  {
    this->get_aspect().get<_log_debug_>()(msg);
  }

  virtual void log_trace(const std::string& msg) const override 
  {
    this->get_aspect().get<_log_trace_>()(msg);
  }
private:
  logger_options _opt;
};

}

#define WJRPC_LOG(T, M, X) { if ( auto log = (T)->get_aspect().template get<M>() ) { std::stringstream ss; ss << X; log(ss.str()); } }

#define WJRPC_LOG_ERROR(T, X)    WJRPC_LOG( T, _log_error_, X )
#define WJRPC_LOG_FATAL(T, X)    WJRPC_LOG( T, _log_fatal_, X )
#define WJRPC_LOG_DEBUG(T, X)    WJRPC_LOG( T, _log_debug_, X )
#define WJRPC_LOG_TRACE(T, X)    WJRPC_LOG( T, _log_trace_, X )
