cmake_minimum_required(VERSION 3.5)

project(plugins)
message(STATUS "Project Plugins")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(SO_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)
set(BUILD_DOXYGEN FALSE)


find_package(Boost 1.58 REQUIRED COMPONENTS
        filesystem
        program_options
        serialization)

if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    MESSAGE("BOOST ROOT SET")
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
    set(Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF")
endif ()


include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/AMQP-CPP/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Net/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/library/poco/Foundation/include)


set(SOURCE_FILES
        src/task.cpp
        src/rabbitmq_worker.cpp
        src/SimplePocoHandler.cpp)


add_subdirectory(library)
add_executable(plugins ${SOURCE_FILES})
target_link_libraries(plugins amqpcpp PocoNet PocoFoundation ${Boost_LIBRARIES})

install(TARGETS plugins DESTINATION bin)

#make package for debian-based distributions
#cd directory build and run command  cpack
if(UNIX AND NOT APPLE)
set(LINUX TRUE)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Postnikov Dmitry")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "C++ code  for RabbitMQ. Start queue name hello, listen queue and send testing data")
set(CPACK_PACKAGE_EXECUTABLES "Plugins" "Plugins")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS} cmake (>=3.5)")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libboost-all-dev (>=1.58)")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libpocofoundation50 (>=50)")
endif()

include(CPack)

find_package(Doxygen)

if (DOXYGEN_FOUND AND ${BUILD_DOXYGEN})

    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target(doc_doxygen ALL
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

endif (DOXYGEN_FOUND AND ${BUILD_DOXYGEN})

