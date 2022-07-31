#include "User.h"

udp::endpoint User::get_endpoint() {
	return *this->endpoint;
}

void User::send_data(boost::asio::mutable_buffer data) {
	this->socket->send_to(data, *this->endpoint);
}

void User::setCharacterPosition(point_t new_position) {
	character.setPosition(new_position);
}

void User::setCharacterTrajectory(linestring_t new_trajectory) {
	character.setTrajectory(new_trajectory);
}
