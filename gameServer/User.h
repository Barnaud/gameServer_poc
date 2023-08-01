#pragma once
#include<iostream>
#include<boost/asio.hpp>
#include "GameObject.h"
#include "geometry_typedef.h"
#include "chrono_typedef.h"

using boost::asio::ip::udp;

class User {

public:
	User(udp::endpoint* endpoint_a, udp::socket* socket_a) :endpoint(endpoint_a), socket(socket_a)
	{
		character = new GameObject();
		std::cout << "Creating user" << std::endl;
	}

	~User() {
		std::cout << "Deleting user" << std::endl;
		delete character;
	}

	void setCharacterPosition(point_t new_position);
	point_t getCharacterPosition();
	void setCharacterTrajectory(linestring_t new_trajectory);
	GameObject* getCharacter();

	void validateReceivedPosition(point_t& received_position, time_point_t& recived_time_point);

	udp::endpoint get_endpoint();
	void send_data(boost::asio::mutable_buffer data);

	unsigned int getCharacterUid();
	void setLastAckedRequestTimestamp(long long new_lastAckedRequestTimestamp);
	time_point_t getLastAckedRequestTimestamp();

private:
	udp::endpoint* endpoint;
	udp::socket* socket;
	GameObject* character;
	timed_point_t previous_position;
	time_point_t lastAckedRequestTimestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(0)); //Timestamp=0
};