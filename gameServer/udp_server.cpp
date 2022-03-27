#include "udp_server.h"
#include <iostream>
#include "Constants.h"
#include <chrono>
#include <thread>
#include "GameObject.h"


using std::chrono::operator""ms;


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
			auto tickStartTime = std::chrono::steady_clock::now();
			udp::endpoint receive_endpoint;
			this->socket->receive_from(boost::asio::buffer(this->receive_buffer), receive_endpoint);
			/*if (!this->endpoint_map[receive_endpoint]) {
				std::cout << "A new client connected, assigning it id: " << std::endl;
			}*/
			std::cout << "got data from endpoint: " << receive_endpoint << "Previous endpoint: " << previous_endpoint<< std::endl;
			for (int i = 0; i < 3; i++) {
				std::cout << "data " << i << ": " << (int) receive_buffer[i] << std::endl;
			}
			if(findUserByEndpoint(receive_endpoint)){
				std::cout << "found user" << std::endl;
			}
			else {
				std::cout << "A new user logged in and was added to" << std::endl;
				logUser(receive_endpoint, *this->socket);
			}
			std::cout << "after catch" << std::endl;
			GameObject::moveAllObjectsOneTick();
			boost::asio::mutable_buffer dataToSend = generateDataToSend();
			for (User oneUser : users) {
				oneUser.send_data(dataToSend);
			}
			std::this_thread::sleep_until(tickStartTime + 1000ms);
			std::cout << "one loop passed" << std::endl;
		}
	}
	catch (std::exception& error) {
		std::cerr << "Error while listening connexions: " << error.what() << std::endl;
	}
}

User* udp_server::findUserByEndpoint(udp::endpoint &tested_endpoint){
	for (int i = 0; i < this->users.size(); i++) {
		if (this->users[i].get_endpoint() == tested_endpoint) {
			return &users[i];
		}
	}
	return NULL;

}

void udp_server::logUser(udp::endpoint& new_endpoint, udp::socket& socket) {
	User new_user = User(&new_endpoint, &socket);
	this->users.push_back(new_user);
	char data[1] = { 1 };
	new_user.send_data(boost::asio::buffer(data));
}

boost::asio::mutable_buffer udp_server::generateDataToSend() {
	std::vector<GameObject*>* allGameObjects = GameObject::getGameObjects();
	char temp_buffer[4];
	std::vector<unsigned char> sendVector({tick_id});
	//Data format: tick_id[1], gameObjectId[4], x[4], y[4], z[4] => buffer size = (gameObjects.size() * 4) + 1
	for (GameObject* oneGameObject : *allGameObjects) {
		point_t position = oneGameObject->getPosition();
		float coordinates[3];
		coordinates[0] = bg::get<0>(position);
		coordinates[1] = bg::get<1>(position);
		coordinates[2] = bg::get<2>(position);
		for (int i = 0; i < 3; i++) {
			memcpy(temp_buffer, &coordinates[i], sizeof(coordinates[i]));
			for (int j = 0; j < 4; j++) {
				sendVector.push_back(temp_buffer[j]);
			}
		}
		return boost::asio::buffer(sendVector);

	}
}
