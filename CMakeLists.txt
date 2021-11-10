cmake_minimum_required(VERSION 3.0.2)
project(zmq_network)

#set(ZeroMQ_DIR /usr/local/lib/)
#find_path(ZMQ_INCLUDE_DIR zmq.h)
#find_library(ZMQ_LIBRARY NAMES zmq)
#set(ZMQ_LIBRARIES ${ZMQ_LIBRARY})
#set(ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

#find_package(libzmq REQUIRED)
#find_package(zmq REQUIRED)

#add_library(zmq INTERFACE)

#target_link_libraries(zmq INTERFACE $<$<CONFIG:Debug>:libzmq>$<$<CONFIG:Release>:libzmq-static>)
#target_include_directories(zmq INTERFACE ${zmq_DIR})
#target_compile_definitions(zmq INTERFACE ZMQ_BUILD_DRAFT_API=1)

#ADD_COMPILE_OPTIONS(-DZMQ_BUILD_DRAFT_API=1)

set(ENABLE_DRAFT ON)

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

find_package(catkin REQUIRED
  COMPONENTS
    roscpp
    rospy
    std_msgs
)

catkin_package(
  INCLUDE_DIRS
    include
  LIBRARIES
    ${PROJECT_NAME}
  CATKIN_DEPENDS
    roscpp
    rospy
    std_msgs
  DEPENDS
    Boost
)

include_directories(
 include
 ${catkin_INCLUDE_DIRS}
 ${cppzmq_INCLUDE_DIRS}
 ${ZeroMQ_INCLUDE_DIRS}
)

set(PROJECT_LIB_FILES
  src/zmq_class.cpp
)

add_library(${PROJECT_NAME}_lib
  ${PROJECT_LIB_FILES}
)

add_executable(${PROJECT_NAME}
  nodes/node.cpp
)

target_link_libraries(${PROJECT_NAME}_lib
  pthread
  stdc++
  ${catkin_LIBRARIES}
  ${cppzmq_LIBRARIES}
  ${ZeroMQ_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME}
  ${PROJECT_NAME}_lib
)

add_dependencies(${PROJECT_NAME}
  ${catkin_EXPORTED_TARGETS}
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
)
