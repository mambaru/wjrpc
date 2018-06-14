

Документация [doxygen](https://mambaru.github.io/wjrpc/index.html).
Репозитарий на [github.com](https://github.com/mambaru/wjrpc).

# Сборка и установка

```bash
Usage:
  make help
  make shared
  make static
  make all
  make doc
  make clean
  make install
```
Если установлен doxygen то в папке `/doc/html` будет документация.

Все примеры с описанием в `examples`.

Гарантированное выполнение заданий:
```cpp
int main()
{
  boost::asio::io_service ios;
  wflow::workflow wf(ios);

  // Простое задание 
  wf.safe_post( [](){ std::cout << "Simple safe post  " << std::endl; } );

  // Отложенное задание 
  wf.safe_post( std::chrono::seconds(4), [](){ std::cout << "Safe post after delay 4 second " << std::endl; } );

  // Задание на конкретный момент времени 
  auto tp = std::chrono::system_clock::now();
  tp += std::chrono::seconds(2);
  wf.safe_post( tp, [](){ std::cout << "Safe post in time point" << std::endl; } );

  // Ожидаем выполнение всех заданий 
  ios.run();
}
```

Пример ограничения размера очереди:
```cpp
int main()
{
  boost::asio::io_service ios;
  wflow::workflow_options opt;
  opt.maxsize = 5;
  wflow::workflow wf(ios, opt);

  for (int i = 0; i < 10; ++i)
  {
    wf.post( 
      [i](){ std::cout << "post " << i << std::endl; },
      [i](){ std::cout << "drop " << i << std::endl; } 
    );
  }

  std::cout << "Run!" << std::endl;
  // Ожидаем выполнение всех заданий 
  ios.run();
}
```

Различные варианты таймеров:

```cpp
  boost::asio::io_service ios;
  wflow::workflow wf(ios);

  // простой таймер
  wf.create_timer(
    std::chrono::seconds(5),
    []()->bool{ 
      std::cout << "Every 5 seconds" << std::endl;
      // Продолжаем работу таймера
      return true;
    }
  );

  // Запускаем пятисекундный таймер через 30 секунд
  wf.create_timer(
    std::chrono::seconds(30),
    std::chrono::seconds(5),
    []()->bool{ 
      std::cout << "Every 5 seconds" << std::endl;
      return true;
    }
  );

  // Запускаем раз в сутки в 5 утра 
  wf.create_timer(
    "05:00:00",
    []()->bool{ 
      std::cout << "Every day at 5:00 am" << std::endl;
      return true;
    }
  );

  // Первый запуск в 3 утра, далее каждые 6 часов
  wf.create_timer(
    "03:00:00",
    std::chrono::hour(6),
    []()->bool{ 
      std::cout << "Every six hour" << std::endl;
      return true;
    }
  );

  ios.run();
```

Пример тестирования пропускной способности (порядка 3 млн в секунду) см. example9 и example10
```cpp
  wflow::asio::io_service ios;
  wflow::asio::io_service::work wrk(ios);
  wflow::workflow_options opt;
  opt.threads = 1;
  wflow::workflow wf(ios, opt);
  wf.start();
  std::atomic<bool> run(true);
  size_t counter = 0;

  std::thread t([&run, &wf, &counter](){
    for (;run; ++counter)
      wf.post([](){});
  });

  wf.safe_post( std::chrono::seconds(10), [&ios, &run](){ ios.stop(); run = false;});
  ios.run();
  t.join();
  std::cout << counter/10 << std::endl;
```
