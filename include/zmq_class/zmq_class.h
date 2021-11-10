#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <thread>
#include <chrono>
#include <mutex>

#include <zmq.hpp>

#include <ros/ros.h>

class ZMQ_CLASS{
public:
  explicit ZMQ_CLASS(ros::NodeHandle nh);
  ~ZMQ_CLASS();

  std::string zipcode_;
  std::string udpsend_group_, udprecv_group_;
  std::string udp_ip_, tcpsub_ip_, tcpreq_ip_;

  bool controlDone_;
  std::string send_req_, recv_req_, recv_sub_, send_rad_, recv_dsh_;
  
private:
  ros::NodeHandle nodeHandle_;

  void init();
  void spin();
  bool readParameters();
  void* subscribeZMQ();
  void* requestZMQ();
  void* udpsendZMQ();
  void* udprecvZMQ();
  
  std::thread subThread_, reqThread_, udpsendThread_, udprecvThread_;
  zmq::context_t context_;
  zmq::socket_t sub_socket_, req_socket_, udpsend_socket_, udprecv_socket_;

};
