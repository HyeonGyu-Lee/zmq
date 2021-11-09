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

class ZMQ_NET{
public:
  explicit ZMQ_NET(ros::NodeHandle nh);
  ~ZMQ_NET();
private:
  ros::NodeHandle nodeHandle_;

  void init();
  void spin();
  bool readParameters();
  void* subscribeZMQ();
  void* requestZMQ();
  void* udpsendZMQ();
  void* udprecvZMQ();

  std::string zipcode_;
  std::string udpsend_group_, udprecv_group_;
  std::string udp_ip_, tcpsub_ip_, tcpreq_ip_;
  std::thread subThread_, reqThread_, udpsendThread_, udprecvThread_;

  zmq::context_t context_;
  zmq::socket_t sub_socket_, req_socket_, udpsend_socket_, udprecv_socket_;

  bool controlDone_;
  std::string send_req_;
  std::string recv_req_;
  std::string recv_sub_;
  std::string send_rad_;
  std::string recv_dsh_;
};
