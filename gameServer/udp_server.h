#pragma once

#include <boost/asio.hpp>
#include <map>
#include "User.h"
#include "Constants.h"

using boost::asio::ip::udp;

class udp_server {

private:
	boost::asio::io_context context;
	udp::endpoint* server_endpoint;
	udp::socket* socket;
	unsigned char receive_buffer[3];
	std::vector<User> users;
	unsigned char tick_id = 0;

	User* findUserByEndpoint(udp::endpoint &tested_endpoint);
	void logUser(udp::endpoint &new_endpoint, udp::socket &socket);

public:

	udp_server(int port);
	void start_listening();
	boost::asio::mutable_buffer generateDataToSend();


};