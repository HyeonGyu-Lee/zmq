#include "zmq_class/zmq_class.h"

ZMQ_NET::ZMQ_NET(ros::NodeHandle nh)
  :nodeHandle_(nh), context_(1)
{
  if(!readParameters())
  {
    ros::requestShutdown();
  }

  init();

  spin();
}

ZMQ_NET::~ZMQ_NET()
{
  std::cout << "Disconnected" << std::endl;
  controlDone_ = true;
  sub_socket_.close();
  req_socket_.close();
  udpsend_socket_.close();
  udprecv_socket_.close();
  context_.close();
}

void ZMQ_NET::init()
{
  controlDone_ = false;

  /* Initialize Socket */
  sub_socket_ = zmq::socket_t(context_, ZMQ_SUB);
  sub_socket_.connect(tcpsub_ip_);
  const char *filter = zipcode_.c_str();
  sub_socket_.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter)); 

  req_socket_ = zmq::socket_t(context_, ZMQ_REQ); 
  req_socket_.connect(tcpreq_ip_);
  //req_socket_.setsockopt(ZMQ_REQ_CORRELATE, 1); 

  printf("TEXT");
  std::cout << "DEBUG";
  std::cout << "DEBUG";
  udpsend_socket_ = zmq::socket_t(context_, ZMQ_RADIO);
  udpsend_socket_.connect(udp_ip_);

  udprecv_socket_ = zmq::socket_t(context_, ZMQ_DISH);
  udprecv_socket_.bind(udp_ip_);
  udprecv_socket_.join(udprecv_group_.c_str());

  subThread_ = std::thread(&ZMQ_NET::subscribeZMQ, this);
  reqThread_ = std::thread(&ZMQ_NET::requestZMQ, this);
  udpsendThread_ = std::thread(&ZMQ_NET::udpsendZMQ, this);
  udprecvThread_ = std::thread(&ZMQ_NET::udprecvZMQ, this);
}

bool ZMQ_NET::readParameters()
{
  std::string tcp_ip, tcpsub_port, tcpreq_port;
  std::string udp_ip, udp_port;

  nodeHandle_.param("tcp_ip/ip_addr",tcp_ip,std::string("tcp://192.168.85.128"));
  nodeHandle_.param("tcp_ip/sub_port",tcpsub_port,std::string("5555"));
  nodeHandle_.param("tcp_ip/req_port",tcpreq_port,std::string("4444"));

  nodeHandle_.param("tcp_ip/zipcode",zipcode_,std::string("00001"));

  nodeHandle_.param("udp_ip/ip_addr",udp_ip,std::string("udp://127.0.0.1"));
  nodeHandle_.param("udp_ip/port",udp_port,std::string("9090"));
  nodeHandle_.param("udp_ip/send_group",udpsend_group_,std::string("FV1"));
  nodeHandle_.param("udp_ip/recv_group",udprecv_group_,std::string("LV"));

  tcpsub_ip_ = tcp_ip;
  tcpsub_ip_.append(":");
  tcpsub_ip_.append(tcpsub_port);
  tcpreq_ip_ = tcp_ip;
  tcpreq_ip_.append(":");
  tcpreq_ip_.append(tcpreq_port);
  udp_ip_ = udp_ip;
  udp_ip_.append(":");
  udp_ip_.append(udp_port);

  return true;
}

void* ZMQ_NET::subscribeZMQ()
{
  while(sub_socket_.connected() && !controlDone_)
  {
    zmq::message_t update;

    sub_socket_.recv(&update, 0);

    recv_sub_ = static_cast<char*>(update.data());
  }
}
void* ZMQ_NET::requestZMQ()
{ 
  int send_data = 0;

  while(req_socket_.connected() && !controlDone_)
  {
    zmq::message_t request(20);

    snprintf((char *) request.data(), 20, "%s %d", send_req_.c_str());
    req_socket_.send(request);

    int zipcode, recv_data;
    zmq::message_t reply;
    bool rc = req_socket_.recv(&reply, 0);

    recv_req_ = static_cast<char*>(reply.data());
  }
}
void* ZMQ_NET::udpsendZMQ()
{
  while(udpsend_socket_.connected() && !controlDone_)
  {
    zmq::message_t request(20);
    request.set_group(udpsend_group_.c_str());

    snprintf((char *) request.data(), 20, "%s", send_rad_.c_str());

    udpsend_socket_.send(request, 0);
  }
}
void* ZMQ_NET::udprecvZMQ()
{
  while(udprecv_socket_.connected() && !controlDone_)
  {
    zmq::message_t reply;

    bool rc = udprecv_socket_.recv(&reply, 0);

    recv_dsh_ = static_cast<char*>(reply.data());
  }
}

void ZMQ_NET::spin()
{
  int cnt = 0;
  static int zipcode, recv_sub, send_req, recv_req, send_rad, recv_dsh;
  std::istringstream iss;

  int send_rad_data = 100;
  int send_req_data = 0;
  while(ros::ok && !controlDone_)
  {
    iss = std::istringstream(recv_sub_);
    iss >> zipcode >> recv_sub;

    send_req_ = boost::str(boost::format("%s %d") % zipcode_.c_str() % send_req_data++);
    iss = std::istringstream(send_req_);
    iss >> zipcode >> send_req;

    iss = std::istringstream(recv_req_);
    iss >> zipcode >> recv_req;

    send_rad_ = boost::str(boost::format("%s %d") % zipcode_.c_str() % send_rad_data++);
    iss = std::istringstream(send_rad_);
    iss >> zipcode >> send_rad;

    iss = std::istringstream(recv_dsh_);
    iss >> zipcode >> recv_dsh;

    printf("\033[2J");
    printf("\033[1;1H");
    printf("%s %d %s\n","- client",cnt++,udp_ip_.c_str());
    printf("%s %s %s %s\n"," udpsend:",udpsend_group_.c_str(),"| udprecv:",udprecv_group_.c_str());
    printf("%15s %s\n","zipcode :", zipcode_.c_str());
    printf("%15s %5d\n","sub recv data :", recv_sub);
    printf("%15s %5d\n","req send data :", send_req);
    printf("%15s %5d\n","req recv data :", recv_req);
    printf("%15s %5d\n","rad send data :", send_rad);
    printf("%15s %5d\n","dsh recv data :", recv_dsh);

    if(ros::isShuttingDown())
    {
      controlDone_ = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
}
