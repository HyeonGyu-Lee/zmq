#include "zmq_net/zmq_net.h"
#include <ros/ros.h>

int main(int argc, char** argv)
{
	ros::init(argc, argv, "Client_node");
	ros::NodeHandle nodeHandle("~");
	ZMQ_NET ZMQNET(nodeHandle);
	ros::spin();
	return 0;
}
