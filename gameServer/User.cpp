#include "User.h"

udp::endpoint User::get_endpoint() {
	return *this->endpoint;
}

void User::send_data(boost::asio::mutable_buffer data) {
	this->socket->send_to(data, *this->endpoint);
}

void User::setCharacterPosition(point_t new_position) {
	character->setPosition(new_position);
}

void User::setCharacterTrajectory(linestring_t new_trajectory) {
	character->setTrajectory(new_trajectory);
}

void User::validateReceivedPosition(point_t& received_position, time_point_t& recived_time_point) {
	return;

}

void User::setLastAckedRequestTimestamp(long long new_lastAckedRequestTimestamp) {
	if (new_lastAckedRequestTimestamp > lastAckedRequestTimestamp) {
		lastAckedRequestTimestamp = new_lastAckedRequestTimestamp;
	}
}


unsigned int User::getCharacterUid() {
	return character->getUid();
}