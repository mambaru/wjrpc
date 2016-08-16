#pragma once 

//#include <iow/workflow/queue_options_json.hpp>
#include <iow/workflow/workflow_options.hpp>
#include <wfc/json.hpp>

namespace iow {
  
struct workflow_options_json
{
  JSON_NAME(threads)
  JSON_NAME(use_io_service)
  JSON_NAME(maxsize)
  JSON_NAME(wrnsize)
  JSON_NAME(show_wrn_ms)

 
  typedef ::wjson::object<
    workflow_options,
    ::wjson::member_list<
      ::wjson::member< n_wrnsize, workflow_options, size_t, &workflow_options::wrnsize >,
      ::wjson::member< n_maxsize, workflow_options, size_t, &workflow_options::maxsize >,
      ::wjson::member< n_show_wrn_ms, workflow_options, time_t, &workflow_options::show_wrn_ms >,
      ::wjson::member< n_threads, workflow_options, int,  &workflow_options::threads >,
      ::wjson::member< n_use_io_service, workflow_options, bool, &workflow_options::use_io_service >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

struct workflow_options2_json
{
  JSON_NAME(post_delay_ms)
  JSON_NAME(rate_limit)

  typedef ::wjson::object<
    workflow_options,
    ::wjson::member_list<
      ::wjson::base< workflow_options_json >,
      ::wjson::member< n_post_delay_ms, workflow_options, time_t, &workflow_options::post_delay_ms >,
      ::wjson::member< n_rate_limit, workflow_options, size_t, &workflow_options::rate_limit >
    >
  > type;

  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

struct workflow_options_on_json
{
  JSON_NAME(threads)
  JSON_NAME(use_io_service)
 
  typedef ::wjson::object<
    workflow_options,
    ::wjson::member_list<
      ::wjson::member< n_threads, workflow_options, int,  &workflow_options::threads >,
      ::wjson::member< n_use_io_service, workflow_options, bool, &workflow_options::use_io_service >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}
