#include "calc/calc1.hpp"
#include "calc/calc_p.hpp"
#include "calc/api/plus_json.hpp"
#include "calc/api/minus_json.hpp"
#include "calc/api/multiplies_json.hpp"
#include "calc/api/divides_json.hpp"

#include <wjrpc/engine.hpp>
#include <wjrpc/handler.hpp>
#include <wjrpc/method.hpp>

#include <iostream>
#include <functional>

namespace service
{
  
JSONRPC_TAG(plus)
JSONRPC_TAG(minus)
JSONRPC_TAG(multiplies)
JSONRPC_TAG(divides)

struct method_list: wjrpc::method_list
<
  wjrpc::target<icalc>,
  wjrpc::invoke_method<_plus_, request::plus_json, response::plus_json, icalc, &icalc::plus>,
  wjrpc::invoke_method<_minus_, request::minus_json, response::minus_json, icalc, &icalc::minus>,
  wjrpc::invoke_method<_multiplies_, request::multiplies_json, response::multiplies_json, icalc, &icalc::multiplies>,
  wjrpc::invoke_method<_divides_, request::divides_json, response::divides_json, icalc, &icalc::divides>
>{};


class handler: public ::wjrpc::handler<method_list> {};

typedef wjrpc::engine<handler> engine_type;
}

namespace gateway
{
  JSONRPC_TAG(plus)
  JSONRPC_TAG(minus)
  JSONRPC_TAG(multiplies)
  JSONRPC_TAG(divides)

  struct method_list: wjrpc::method_list
  <
    wjrpc::call_method<_plus_, request::plus_json, response::plus_json>,
    wjrpc::call_method<_minus_, request::minus_json, response::minus_json>,
    wjrpc::call_method<_multiplies_, request::multiplies_json, response::multiplies_json>,
    wjrpc::call_method<_divides_, request::divides_json, response::divides_json>
  >
  {};

  class handler
    : public ::wjrpc::handler<method_list>
    , public icalc
  {
  public:
    virtual void plus( request::plus::ptr req, response::plus::callback cb)  override
    {
      this->template call<_plus_>( std::move(req), cb, nullptr );
    }

    virtual void minus( request::minus::ptr req, response::minus::callback cb) override
    {
      this->template call<_minus_>( std::move(req), cb, nullptr );
    }

    virtual void multiplies( request::multiplies::ptr req, response::multiplies::callback cb) override
    {
      this->template call<_multiplies_>( std::move(req), cb, nullptr );
    }

    virtual void divides( request::divides::ptr req, response::divides::callback cb) override
    {
      this->template call<_divides_>( std::move(req), cb, nullptr );
    }
  };

  typedef wjrpc::engine<handler> engine_type;
}

int main()
{
  // Прокси N1
  auto prx1 = std::make_shared<calc_p>();
  // Шлюз
  auto gtw = std::make_shared<gateway::engine_type>();
  // Сервис
  auto srv = std::make_shared<service::engine_type>();
  // Прокси N2
  auto prx2 = std::make_shared<calc_p>();
  // Калькулятор
  auto clc = std::make_shared<calc1>();
  
  // Связываем второй прокси с калькулятором
  prx2->initialize(clc);
  
  // Связываем сервис со вторым прокси и запускаем сервис
  service::engine_type::options_type srv_opt;
  srv_opt.target = prx2;
  srv->start(srv_opt, 11);

  // Запускаем шлюз
  gateway::engine_type::options_type cli_opt;
  gtw->start(cli_opt, 22);

  // Регистрируем обработчик шлюза и связываем его с серивисом
  gtw->reg_io(33, [srv]( wjrpc::data_ptr d, wjrpc::io_id_t /*io_id*/, wjrpc::output_handler_t handler)
  {
    std::cout << " REQUEST: " << std::string( d->begin(), d->end() ) << std::endl;
    srv->perform_io(std::move(d), 44, [handler](wjrpc::data_ptr d)
    {
      // Переопределение обработчика для вывода JSON-RPC ответа
      std::cout << " RESPONSE: " << std::string( d->begin(), d->end() ) << std::endl;
      handler(std::move(d) );
    });
  });
  
  // Находим зарегистрированный обработчик шлюза по его ID 
  auto gtwh = gtw->find(33);
  // Связываем обработчик шлюза с первм прокси
  prx1->initialize(gtwh);
  
  // Вызываем plus через прокси (prx1->gtw->srv->prx2->clc)
  auto plus = std::make_unique<request::plus>();
  plus->first = 1;
  plus->second = 2;
  prx1->plus( std::move(plus), [](response::plus::ptr res)
  {
    std::cout << "1+2=" << res->value << std::endl;; 
  });

  // Вызываем plus через шлюз (gtw->srv->prx2->clc)
  auto minus = std::make_unique<request::minus>();
  minus->first = 4;
  minus->second = 3;
  gtwh->minus( std::move(minus), [](response::minus::ptr res)
  {
    std::cout << "4-3=" << res->value << std::endl;; 
  });
  
  // Прокси инкрементирует параметры запроса метода plus и результата ответа на него
  /*
  REQUEST: {"jsonrpc":"2.0","method":"plus","params":{"first":2,"second":3},"id":1}
  RESPONSE: {"jsonrpc":"2.0","result":{"value":8},"id":1}
  1+2=9
  REQUEST: {"jsonrpc":"2.0","method":"minus","params":{"first":4,"second":3},"id":2}
  RESPONSE: {"jsonrpc":"2.0","result":{"value":1},"id":2}
  4-3=1
  */
}
