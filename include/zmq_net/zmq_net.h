#include "zmq_class/zmq_class.h"

class ZMQ_NET{
public:
  ZMQ_NET(ros::NodeHandle nh);
  ~ZMQ_NET();
private:
  ZMQ_CLASS ZMQ_SOCKET_;
  void display();
};
