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

point_t User::getCharacterPosition() {
	return character->getPosition();
}

GameObject* User::getCharacter() {
	return character;
}

void User::setCharacterTrajectory(linestring_t new_trajectory) {
	character->setTrajectory(new_trajectory);
}

void User::validateReceivedPosition(point_t& received_position, time_point_t& recived_time_point) {
	return;

}

// Set the timestamp of the last time the client acknoledged a server-sent gamestate
void User::setLastAckedRequestTimestamp(long long new_lastAckedRequestTimestamp) {
	time_point_t new_lastAckedRequestTimestamp_time_point = std::chrono::system_clock::time_point(std::chrono::milliseconds(new_lastAckedRequestTimestamp));
	if (new_lastAckedRequestTimestamp_time_point > lastAckedRequestTimestamp) {
		lastAckedRequestTimestamp = new_lastAckedRequestTimestamp_time_point;
	}
}

time_point_t User::getLastAckedRequestTimestamp() {
	return lastAckedRequestTimestamp;
}


unsigned int User::getCharacterUid() {
	return character->getUid();
}