#pragma once

#include <boost/asio.hpp>
#include <map>

using boost::asio::ip::udp;

class udp_server {

private:
	boost::asio::io_context context;
	udp::endpoint* server_endpoint;
	udp::socket* socket;
	unsigned char receive_buffer[3];
	std::map<udp::endpoint, int> endpoint_map;

public:

	udp_server(int port);
	void start_listening();


};