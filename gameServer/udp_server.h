#pragma once

#include <boost/asio.hpp>
#include <map>
#include "User.h"
#include "Constants.h"
#include <future>
#include <mutex>
#include <vector>

using boost::asio::ip::udp;

class udp_server {

private:
	boost::asio::io_context context;
	udp::socket* socket;
	udp::endpoint* server_endpoint;
	udp::endpoint receive_endpoint;
	unsigned char receive_buffer[128];

	std::vector<User> users;
	unsigned char tick_id = 0;

	std::future<void> f_orchestration;

	std::mutex user_mutex;

	User* findUserByEndpoint(udp::endpoint &tested_endpoint);
	void logUser(udp::endpoint &new_endpoint, udp::socket &socket);
	void start_socket_receive();
	void route_received_data();
	void on_socket_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
	static void orchestrate_object_movements(udp_server* server);

public:

	udp_server(int port);
	void start_listening();
	std::vector<unsigned char> formatGameStateToSend();


};