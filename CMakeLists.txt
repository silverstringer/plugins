cmake_minimum_required(VERSION 3.5)
project(plugins)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

add_library(task.cpp
        rabbitmq_worker.cpp
        SimplePocoHandler.cpp
        ${HEADERS} rabbitmq_worker.hpp thread_safe.hpp SimplePocoHandler.h task.h)

#find_package(Poco REQUIRED Foundation Net NetSSL)
add_executable(task  task.cpp SimplePocoHandler.cpp  rabbitmq_worker.cpp)
target_link_libraries(task amqpcpp PocoNet PocoFoundation)
