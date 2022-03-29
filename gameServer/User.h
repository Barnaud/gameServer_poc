#pragma once
#include<iostream>
#include<boost/asio.hpp>
#include "GameObject.h"
#include "geometry_typedef.h"

using boost::asio::ip::udp;

class User {

public:
	User(udp::endpoint* endpoint_a, udp::socket* socket_a) :endpoint(endpoint_a), socket(socket_a)
	{
		std::cout << "Creating user" << std::endl;
	}

	~User() {
		std::cout << "Deleting user" << std::endl;
	}

	void setCharacterPosition(point_t new_position);

	udp::endpoint get_endpoint();
	void send_data(boost::asio::mutable_buffer data);

private:
	udp::endpoint* endpoint;
	udp::socket* socket;
	GameObject character;

};