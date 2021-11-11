#include "zmq_net/zmq_net.h"

ZMQ_NET::ZMQ_NET(ros::NodeHandle nh)
  :ZMQ_SOCKET_(nh)
{
  display();
}

ZMQ_NET::~ZMQ_NET()
{
  std::cout << "Error" << std::endl;
}

void ZMQ_NET::display()
{
  int cnt = 0;
  static int zipcode, recv_sub, send_req, recv_req, send_rad, recv_dsh;
  std::istringstream iss;
  std::string ipAddr = ZMQ_SOCKET_.getIPAddress();

  int send_rad_data = 100;
  int send_req_data = 0;
  while(!ZMQ_SOCKET_.controlDone_)
  {
    iss = std::istringstream(ZMQ_SOCKET_.recv_sub_);
    iss >> zipcode >> recv_sub;

    ZMQ_SOCKET_.send_req_ = boost::str(boost::format("%s %d") % ZMQ_SOCKET_.zipcode_.c_str() % send_req_data++);
    iss = std::istringstream(ZMQ_SOCKET_.send_req_);
    iss >> zipcode >> send_req;

    iss = std::istringstream(ZMQ_SOCKET_.recv_req_);
    iss >> zipcode >> recv_req;

    ZMQ_SOCKET_.send_rad_ = boost::str(boost::format("%s %d") % ZMQ_SOCKET_.zipcode_.c_str() % send_rad_data++);
    iss = std::istringstream(ZMQ_SOCKET_.send_rad_);
    iss >> zipcode >> send_rad;

    iss = std::istringstream(ZMQ_SOCKET_.recv_dsh_);
    iss >> zipcode >> recv_dsh;

    printf("\033[2J");
    printf("\033[1;1H");
    printf("%s %d (%s) - %s\n","- client",cnt++, ipAddr.c_str(), ZMQ_SOCKET_.udp_ip_.c_str());
    printf("%s %s %s %s\n"," udpsend:",ZMQ_SOCKET_.rad_group_.c_str(),"| udprecv:",ZMQ_SOCKET_.dsh_group_.c_str());
    printf("%15s %s\n","zipcode :", ZMQ_SOCKET_.zipcode_.c_str());
    printf("%15s %5d\n","sub recv data :", recv_sub);
    printf("%15s %5d\n","req send data :", send_req);
    printf("%15s %5d\n","req recv data :", recv_req);
    printf("%15s %5d\n","rad send data :", send_rad);
    printf("%15s %5d\n","dsh recv data :", recv_dsh);

    if(ros::isShuttingDown())
    {
      ZMQ_SOCKET_.controlDone_ = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
}
