## C++ code for RabbitMQ
## Requirements
  
- C++14 compiler support
- Cmake 
- Doxygen
- Poco
- AMQP-CPP 
- Boost

## Build
  git clone https://github.com/silverstringer/plugins.git

  cd plugins/library

  git submodule update --init --recursive

  cd plugins

  mkdir build &&  cd build 

  cmake .. 

  make -j4 
  
## Install
  make install

## Build from script
./build.sh

RUN:

./build.sh -start

##Usage Dockerfile
docker run -it -p 5672:5672 -p  15672:15672 -h localhost rabbitmq:3


docker build -t docker-plugin1 .

docker run -it --rm --network=host docker-plugin1:latest





## Usage
Start rabbitmq-server with default parametrs

1.send message to queue 

2.listen to rb_queue

## License

[MIT](https://github.com/silverstringer/plugins/blob/master/LICENSE)
