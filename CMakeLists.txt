cmake_minimum_required(VERSION 3.5)

project(plugins)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 1)
set (SO_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/AMQP-CPP/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Net/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Foundation/include)


set(SOURCE_FILES
        task.cpp
        rabbitmq_worker.cpp
        SimplePocoHandler.cpp)

#add_library(${SOURCE_FILES} ${HEADERS} rabbitmq_worker.hpp thread_safe.hpp SimplePocoHandler.h task.h)

add_subdirectory( library )
add_executable(plugins ${SOURCE_FILES})
target_link_libraries(plugins amqpcpp PocoNet PocoFoundation)
