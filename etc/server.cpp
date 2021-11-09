#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>

#include <zmq.hpp>

#define ZMQ_BUILD_DRAFT_API

std::mutex mtx;

std::string getIPAddress(){
    std::string ipAddress="Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if(strcmp(temp_addr->ifa_name, "ens33")==0){
                    ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}

void publishFunction(const std::string& uri, int* send_pub)
{
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.bind(uri);

    int zipcode, data;

    zipcode = 0;
    data = 1;
    
    while(publisher.connected()) {
        zmq::message_t message(20);

	snprintf((char *) message.data(), 20, "%05d %d", zipcode, data);
	publisher.send(message);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	
	mtx.lock();
	*send_pub = data;
	mtx.unlock();
	
	zipcode++;
	if(zipcode == 3)
		zipcode = 0;
	data++;
    }
}

void replyFunction(const std::string& uri, int* send_rep, int* recv_rep)
{
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind(uri);

    while(socket.connected()) {
        zmq::message_t request;

	bool rc = socket.recv(&request);
	
	int zipcode, data;

	std::istringstream iss(static_cast<char*>(request.data()));
	iss >> zipcode >> data;

	mtx.lock();
	*recv_rep = data;
	mtx.unlock();

	zmq::message_t reply(20);

	printf("%d",zipcode);	
	switch(zipcode)
	{
		case 0:
		data+=2;
		break;
		case 1:
		data*=2;
		break;
		case 2:
		data-=2;
		break;
		default:
		break;
	}

	snprintf((char *) reply.data(), 20, "%05d %d", zipcode, data);

	mtx.lock();
	socket.send(reply);
	mtx.unlock();

	*send_rep = data;
    }
}

int main(int argv, char** argc)
{
    int send_pub, send_rep, recv_rep;
    std::thread publishThread(publishFunction, "tcp://*:5555", &send_pub);
    std::thread replyThread(replyFunction, "tcp://*:4444", &send_rep, &recv_rep);

    send_rep = 0;
    recv_rep = 0;

    while(1)
    {
	mtx.lock();
	printf("\033[2J");
	printf("\033[1;1H");
	printf("%s (%s)\n","- server", getIPAddress().c_str());
	printf("%15s %5d\n","pub send data :", send_pub);
	printf("%15s %5d\n","rep send data :", send_rep);
	printf("%15s %5d\n","rep recv Data :", recv_rep);
	mtx.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    return 0;
}
