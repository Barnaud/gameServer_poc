#include "User.h"

udp::endpoint User::get_endpoint() {
	return *this->endpoint;
}

void User::send_data(boost::asio::mutable_buffer data) {
	this->socket->send_to(boost::asio::buffer({ 1 }), *this->endpoint);
}