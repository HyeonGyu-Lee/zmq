#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include <zmq.hpp>

int send_req_;
int recv_req_;
int recv_sub_;
int send_rad_;
int recv_dsh_;
char* zipcode_;

void subscribeFunction(const std::string& uri)
{
	zmq::context_t context (1);
	zmq::socket_t subscriber (context, ZMQ_SUB);
	subscriber.connect(uri);

	const char *filter = zipcode_;
	subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

	while(subscriber.connected())
	{
		zmq::message_t update;
		int zipcode, data;

		bool rc = subscriber.recv(&update, 0);
		std::istringstream iss(static_cast<char*>(update.data()));
		iss >> zipcode >> data;

		recv_sub_ = data;
	}
}

void requestFunction(const std::string& uri)
{
	zmq::context_t context(1);

	zmq::socket_t socket(context, ZMQ_REQ);

	socket.connect(uri);

	int zipcode;
	int send_data = 0;
	int recv_data = 0;

	//socket.setsockopt(ZMQ_REQ_CORRELATE, 1);

	while(socket.connected())
	{
		zmq::message_t request(20);

		snprintf((char *) request.data(), 20, "%s %d", zipcode_, send_data);
		
		socket.send(request);
		send_req_ = send_data++;

		zmq::message_t reply;
		bool rc = socket.recv(&reply, 0);

		std::istringstream iss(static_cast<char*>(reply.data()));
		iss >> zipcode >> recv_data;
		
		recv_req_ = recv_data;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void udpsendFunction(const std::string& uri)
{
	zmq::context_t context(1);
	zmq::socket_t socket (context, ZMQ_RADIO);
	socket.connect(uri);

	int send_data = 100;

	while(socket.connected())
	{
		zmq::message_t request(20);
		request.set_group("test");

		snprintf((char *) request.data(), 20, "%s %d", zipcode_, send_data);

		socket.send(request, 0);
		
		send_rad_ = send_data++;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}

void udprecvFunction(const std::string& uri)
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_DISH);
	socket.bind(uri);
	socket.join("test");

	int zipcode, recv_data;

	while(socket.connected())
	{
		zmq::message_t reply;

		bool rc = socket.recv(&reply, 0);

		std::istringstream iss(static_cast<char*>(reply.data()));

		iss >> zipcode >> recv_data;

		recv_dsh_ = recv_data;
	}

}

int main(int argc, char** argv)
{
	std::string udpsend, udprecv;
	if(argc>3)
	{
		zipcode_ = argv[1];
		std::string uri("udp://127.0.0.1:");
		udpsend.append(uri);
		udpsend.append(argv[2]);
		udprecv.append(uri);
		udprecv.append(argv[3]);
	}
	else
	{
		printf("usage ./client [zipcode] [udp send port] [udp recv port]\n");
		return -1;
	}

	std::thread subscribeThread(subscribeFunction, "tcp://192.168.85.128:5555");
	std::thread requestThread(requestFunction, "tcp://192.168.85.128:4444");
	std::thread udpsendThread(udpsendFunction, udpsend);
	std::thread udprecvThread(udprecvFunction, udprecv);

	while(1)
	{
		printf("\033[2J");
		printf("\033[1;1H");
		printf("%s %s%s%s%s%s\n","- client","[udp send port(",argv[2],")|udp recv port(",argv[3],")]");
		printf("%15s %s\n","zipcode :", zipcode_);
		printf("%15s %5d\n","req send data :", send_req_);
		printf("%15s %5d\n","req recv data :", recv_req_);
		printf("%15s %5d\n","sub recv data :", recv_sub_);
		printf("%15s %5d\n","rad send data :", send_rad_);
		printf("%15s %5d\n","dsh recv data :", recv_dsh_);
		std::this_thread::sleep_for(std::chrono::milliseconds(33));
	}

	return 0;
}
