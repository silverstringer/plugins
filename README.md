## C++ code for RabbitMQ
## Requirements
  
- C++11 compiler support
- Cmake 
- Poco
- AMQP-CPP 
- BOOST

## Build
  git clone https://github.com/silverstringer/plugins.git

  cd plugins/library

  git submodule update --init --recursive

  cd plugins

  mkdir build 

  cd build 

  cmake .. 

  make -j4
  
## Usage
Start rabbitmq-server with default parametrs

1.send message to queue 

2.listen to rb_queue
