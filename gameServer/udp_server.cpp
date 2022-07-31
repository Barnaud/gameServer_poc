#include "udp_server.h"
#include <iostream>
#include "Constants.h"
#include <chrono>
#include <thread>
#include "GameObject.h"
#include <boost/bind/bind.hpp>
#include "sleep.h"



//implement lock on user list read/write (orchestrate/readUsers)
//implement lock on gameobject position/trajectory

/*
	Data format : requestType[1], {data}
	resquestType = 1: User login
	requestType = 2: get RTT (server request and response):
	clientTimestamp[8]
	requestType = 3: Sending server-computed gameState
	tick_id[1], gameObjectId[4], x[4], y[4], z[4] => buffer size = (gameObjects.size() * 4) + 1

*/


//using std::chrono::operator""ms;

typedef std::chrono::duration<int, std::milli> milliseconds_type;


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
		start_socket_receive();
		f_orchestration = std::async(std::launch::async, orchestrate_object_movements, this);
		std::cout << "Started orchestration" << std::endl;
		this->context.run();
		std::cout << "After run" << std::endl;
	}
	catch (std::exception& error) {
		std::cerr << "Error while listening connexions: " << error.what() << std::endl;
	}
}

void udp_server::start_socket_receive() {
	this->socket->async_receive_from(boost::asio::buffer(this->receive_buffer), this->receive_endpoint, boost::bind(&udp_server::on_socket_receive, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

}

void udp_server::route_received_data() {
	if (receive_buffer && receive_buffer[0]) {
		switch (receive_buffer[0])
		{
		case 1:
		{
			//LogUser
			if (findUserByEndpoint(receive_endpoint)) {
				//std::cout << "found user" << std::endl;
				std::cout << "Trying to re-log already existing user. Ignoring...";
			}
			else {
				std::cout << "A new user logged in and was added" << std::endl;
				logUser(receive_endpoint, *this->socket);
			}
		}
		case 2:
		{
			auto serverTime = std::chrono::system_clock::now();
			//auto serverTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(serverTime.time_since_epoch()).count();
			long long serverTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(serverTime.time_since_epoch()).count();

			//int serverTimeBuffer[1] = { serverTimeMs };
			std::cout << "Pause" << std::endl;
			this->socket->send_to(boost::asio::buffer({serverTimeMs}), this->receive_endpoint);
		}
		default:
			std::cout << "Received unknown request type (" <<receive_buffer[0]<< ").ignoring..." << std::endl;
			break;
		}
	}
	else {
		std::cout << "Received empty buffer. Ignoring...";
	}

}

void udp_server::on_socket_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if (!error) {

		//DEBUG: Read receivedData
		////std::cout << "got data from endpoint: " << receive_endpoint << std::endl;
		//for (int i = 0; i < 3; i++) {
		//	//std::cout << "data " << i << ": " << (int)receive_buffer[i] << std::endl;
		//}
		route_received_data();
		udp_server::start_socket_receive();
	}

}

void udp_server::orchestrate_object_movements(udp_server* server) {
	auto tickStartTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
	while (true) {
		GameObject::moveAllObjectsOneTick();
		sleep_until(tickStartTime + std::chrono::microseconds(1000000 / tick_rate));
		tickStartTime = std::chrono::high_resolution_clock::now();
		//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(tickStartTime - begin).count() << "[µs]" << std::endl;
		begin = std::chrono::high_resolution_clock::now();

		if (server->users.size()) {
			std::vector<unsigned char> dataToSend = server->formatGameStateToSend();
			server->user_mutex.lock();
			for (User& oneUser : server->users) {
				oneUser.send_data(boost::asio::buffer(dataToSend));
			}
			server->user_mutex.unlock();
		}
		
		//std::cout << "one loop passed" << std::endl;
	}
}

User* udp_server::findUserByEndpoint(udp::endpoint &tested_endpoint){
	user_mutex.lock();
	for (int i = 0; i < this->users.size(); i++) {
		if (this->users[i].get_endpoint() == tested_endpoint) {
			user_mutex.unlock();
			return &users[i];
		}
	}
	user_mutex.unlock();
	return NULL;

}

void udp_server::logUser(udp::endpoint& new_endpoint, udp::socket& socket) {
	user_mutex.lock();
	User* new_user = new User(&new_endpoint, &socket);
	new_user->setCharacterPosition(point_t(0, 0, 0));
	linestring_t new_trajectory = linestring_t();
	bg::append(new_trajectory, point_t(0, 0, 10));
	bg::append(new_trajectory, point_t(5, 0, 5));
	new_user->setCharacterTrajectory(new_trajectory);
	this->users.push_back(*new_user);
	char data[1] = { 1 };
	new_user->send_data(boost::asio::buffer(data));
	user_mutex.unlock();
}

std::vector<unsigned char> udp_server::formatGameStateToSend() {
	std::vector<GameObject*>* allGameObjects = GameObject::getGameObjects();
	unsigned char temp_buffer[4];
	std::vector<unsigned char> sendVector({(char)3, tick_id});
	//Data format: requestType[1] = 3, tick_id[1], gameObjectId[4], x[4], y[4], z[4] => buffer size = (gameObjects.size() * 4) + 1
	for (GameObject* oneGameObject : *allGameObjects) {
		point_t position = oneGameObject->getPosition();
		//std::cout << "coordinates to send for one object: " << std::endl;
		float coordinates[3];

		unsigned int objectId = oneGameObject->getUid();
		memcpy(temp_buffer, &objectId , sizeof(objectId));
		for (int k = 0; k < sizeof(objectId); k++) {
			sendVector.push_back(temp_buffer[k]);
		}
		
		coordinates[0] = bg::get<0>(position);
		coordinates[1] = bg::get<1>(position);
		coordinates[2] = bg::get<2>(position);
		for (int i = 0; i < 3; i++) {
			//std::cout << coordinates[i] << std::endl;
			memcpy(temp_buffer, &coordinates[i], sizeof(coordinates[i]));
			for (int j = 0; j < 4; j++) {
				sendVector.push_back(temp_buffer[j]);
			}
			//sendVector.push_back(coordinates[i]);
		}

	}
	return sendVector;
}
