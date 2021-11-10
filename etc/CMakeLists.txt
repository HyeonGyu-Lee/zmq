cmake_minimum_required(VERSION 2.8.3)
project(zmqnet)

set(ENABLE_DRAFTS ON)

add_definitions(-DZMQ_BUILD_DRAFT_API=1)

set(ZeroMQ_DIR /usr/local/lib/)
find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)
set(ZeroMQ_LIBRARIES ${ZMQ_LIBRARY})
set(ZeroMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

set(cppzmq_DIR /usr/local/share/cmake/cppzmq)
#find_package(cppzmq REQUIRED)

add_library(cppzmq INTERFACE)

target_link_libraries(cppzmq INTERFACE $<$<CONFIG:Debug>:libzmq>$<$<CONFIG:Release>:libzmq-static>)
target_include_directories(cppzmq INTERFACE ${cppzmq_DIR})
target_compile_definitions(cppzmq INTERFACE ZMQ_BUILD_DRAFT_API=1)

find_package(Boost REQUIRED python
  COMPONENTS
    thread
)

include_directories(
  ${cppzmq_INCLUDE_DIRS}
  ${ZeroMQ_INCLUDE_DIRS}
)

set(PROJECT_LIB_FILES
  server.cpp
)

add_library(${PROJECT_NAME}_lib
  ${PROJECT_LIB_FILES}
)

add_executable(server
  server.cpp
)

add_executable(client
  client.cpp
)

target_link_libraries(${PROJECT_NAME}_lib
  pthread
  stdc++
  ${cppzmq_LIBRARIES}
  ${ZeroMQ_LIBRARIES}
)

target_link_libraries(server
  ${PROJECT_NAME}_lib
)

target_link_libraries(server
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
)

target_link_libraries(client
  ${PROJECT_NAME}_lib
)

target_link_libraries(client
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
)
