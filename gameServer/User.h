#pragma once
#include<iostream>
#include<boost/asio.hpp>
#include "GameObject.h"
#include "geometry_typedef.h"
#include "chrono_typedef.h"

using boost::asio::ip::udp;

class User {

public:
	User(std::shared_ptr<udp::endpoint> endpoint_a, udp::socket* socket_a) :endpoint(endpoint_a), socket(socket_a)
	{
		character = new GameObject();
		std::cout << "Creating user" << std::endl;
	}

	~User() {
		std::cout << "Deleting user" << std::endl;
		delete character;
	}

	void setCharacterPosition(GameObjectPosition new_position);
	GameObjectPosition getCharacterPosition();
	void setCharacterTrajectory(linestring_t new_trajectory);
	GameObject* getCharacter();

	void validateReceivedPosition(point_t& received_position, time_point_t& recived_time_point);

	udp::endpoint get_endpoint();
	void send_data(boost::asio::mutable_buffer data);

	unsigned int getCharacterUid();
	void setLastAckedRequestTimestamp(long long new_lastAckedRequestTimestamp);
	time_point_t getLastAckedRequestTimestamp();

private:
	std::shared_ptr<udp::endpoint> endpoint;
	udp::socket* socket;
	GameObject* character;
	time_point_t lastAckedRequestTimestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(0)); //Timestamp=0
};