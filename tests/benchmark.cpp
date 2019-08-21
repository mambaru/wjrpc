#include <wjson/json.hpp>
#include <wjrpc/outgoing/outgoing_result_json.hpp>

#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wjrpc/memory.hpp>
#include <chrono>
#include <iostream>

typedef std::vector<int> param_type;
typedef std::unique_ptr<param_type> param_ptr;
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef wjson::array< std::vector< wjson::value<int> > > param_json;
void test1();
void test2();
void test3();
void ttt(std::function<void()> f);
param_type params1 = {1,2,3,4,5};

void test1()
{
  data_type output;
  output.reserve(100);
  auto start = ::std::chrono::high_resolution_clock::now();
  int count = 10000;
  for (int i=0; i < count; ++i)
  {
    output.clear();
    wjrpc::outgoing_result_json<param_json>::serializer serializer;
    wjrpc::outgoing_result<param_type> result;
    result.result = std::make_unique<param_type>(params1);
    serializer(result, std::back_inserter(output));
    //!serializer(result, std::back_inserter(output));
  }
  auto finish = ::std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast< std::chrono::microseconds >(finish - start).count();
  float rate = float(count*1000000.0) / float(span);
  std::cout << std::string( output.begin(), output.end() ) << std::endl;
  std::cout << "test1: " << span << " microseconds " << rate << " persec" << std::endl;
}

void ttt(std::function<void()> f)
{
  f();
}

template<typename F>
void ttt2(F f)
{
  f();
}

void test2()
{
  data_type output;
  output.reserve(100);
  long count = 10000;
  auto start = ::std::chrono::high_resolution_clock::now();
  for (long i=0; i < count; ++i)
  {
    auto params = std::make_unique<param_type>(params1);
    auto pparams = std::make_shared<param_ptr>( std::move(params) );
    auto ser = [&output, pparams](){
      output.clear();
      wjrpc::outgoing_result_json<param_json>::serializer serializer;
      wjrpc::outgoing_result<param_type> result;
      result.result = std::move(*pparams);
      serializer(result, std::back_inserter(output));
    };
    //ser();
    //ttt2(std::move(ser));
    //ttt2(ser);
    ttt(ser);
  }
  auto finish = ::std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast< std::chrono::microseconds >(finish - start).count();
  float rate = float(count)*float(1000000) / float(span);
  std::cout << std::string( output.begin(), output.end() ) << std::endl;
  std::cout << "test2: "<< span << " microseconds " << rate << " persec" << std::endl;
}


void test3()
{
  data_type output;
  data_type outpar;
  output.reserve(100);
  auto start = ::std::chrono::high_resolution_clock::now();
  long count = 10000;
  for (long i=0; i < count; ++i)
  {
    output.clear();
    outpar.clear();
    outpar.reserve(100);
    wjrpc::outgoing_result<data_type> result;
    wjrpc::outgoing_result_json< wjson::raw_value< data_type > >::serializer serializer;
    param_json::serializer()( params1, std::back_inserter(outpar));
    result.result = std::make_unique<data_type>( std::move(outpar) );
    //result.result = nullptr;
    serializer(result, std::back_inserter(output));
  }
  auto finish = ::std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast< std::chrono::microseconds >(finish - start).count();
  float rate = float(count)*float(1000000) / float(span);
  std::cout << std::string( output.begin(), output.end() ) << std::endl;
  std::cout << "test3: " << span << " microseconds " << rate << " persec" << std::endl;
}

int main()
{
  for (int i=0; i < 10000; ++i)
  {
    test1();
    test2();
    test3();
  }
  return 0;
}
