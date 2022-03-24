#include "udp_server.h"
#include <iostream>


udp_server::udp_server(int port) {

	std::cout << "Will init server on port: " << port << std::endl;
	try {
		this->server_endpoint = new udp::endpoint(udp::v4(), port);
		this->socket = new udp::socket(this->context, *this->server_endpoint);
	}
	catch (std::exception& err) {
		std::cerr << "Error while initializing server: " << err.what() << std::endl;
	}
}

void udp_server::start_listening() {
	std::cout << "Listening to incoming connexions" << std::endl;
	try {
		udp::endpoint previous_endpoint;
		while (true) {
			udp::endpoint receive_endpoint;
			this->socket->receive_from(boost::asio::buffer(this->receive_buffer), receive_endpoint);
			/*if (!this->endpoint_map[receive_endpoint]) {
				std::cout << "A new client connected, assigning it id: " << std::endl;
			}*/
			std::cout << "got data from endpoint: " << receive_endpoint << "Previous endpoint: " << previous_endpoint<< std::endl;
			for (int i = 0; i < 3; i++) {
				std::cout << "data " << i << ": " << (int) receive_buffer[i] << std::endl;
			}
			if (receive_endpoint == previous_endpoint) {
				std::cout << "Previous endpoint is the same as before" << std::endl;
			}
			previous_endpoint = receive_endpoint;
		}
	}
	catch (std::exception& error) {
		std::cerr << "Error while listening connexions: " << error.what() << std::endl;
	}
}
