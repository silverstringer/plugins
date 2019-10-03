cmake_minimum_required(VERSION 3.5)

project(plugins)
message(STATUS "Project Plugins")

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 1)
set (SO_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)
#set(BUILD_DOXYGEN FALSE CACHE BOOL "Build doxygen documentation on every make")


find_package(Boost 1.58 REQUIRED COMPONENTS
        filesystem
        program_options
        serialization)

if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
    MESSAGE("BOOST ROOT SET")
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
    set( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )
endif ()


include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/AMQP-CPP/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Net/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Foundation/include)


set(SOURCE_FILES
        src/task.cpp
        src/rabbitmq_worker.cpp
        src/SimplePocoHandler.cpp)


add_subdirectory( library )
add_executable(plugins ${SOURCE_FILES})
target_link_libraries(plugins amqpcpp PocoNet PocoFoundation  ${Boost_LIBRARIES})

install (TARGETS plugins DESTINATION bin)