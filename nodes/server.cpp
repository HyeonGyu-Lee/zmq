#include "zmq_class/zmq_class.h"
#include <ros/ros.h>

int main(int argc, char** argv)
{
	ros::init(argc, argv, "Server_node");
	ros::NodeHandle nodeHandle("~");
	ZMQ_NET ZMQNET(nodeHandle);
	ros::spin();
	return 0;
}
