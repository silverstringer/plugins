## C++ code for RabbitMQ
## Requirements
  
- C++11 compiler support
- Poco
- AMQP-CPP 
- Cmake

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

send message to queue

