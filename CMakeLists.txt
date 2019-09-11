cmake_minimum_required(VERSION 3.5)

project(plugins)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 1)
set (SO_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/AMQP-CPP/include)

add_library(task.cpp
        rabbitmq_worker.cpp
        SimplePocoHandler.cpp
        ${HEADERS} rabbitmq_worker.hpp thread_safe.hpp SimplePocoHandler.h task.h)

#find_package(Poco REQUIRED Foundation Net NetSSL)
add_subdirectory( library )
add_executable(task  task.cpp SimplePocoHandler.cpp  rabbitmq_worker.cpp)
target_link_libraries(task amqpcpp PocoNet PocoFoundation)
