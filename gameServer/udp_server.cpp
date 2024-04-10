#include "udp_server.h"
#include <iostream>
#include "Constants.h"
#include <chrono>
#include <thread>
#include <memory>
#include "GameObject.h"
#include <boost/bind/bind.hpp>
#include "timeUtils.h"
#include "ClientBuffer.h"

#include "chrono_typedef.h"


//implement lock on user list read/write (orchestrate/readUsers)
//implement lock on gameobject position/trajectory

/*
	Data format : requestType[1], {data}
	resquestType = 1: User login
	request_id[1], request_id[1], char_type[4]
	requestType = 2: get RTT (server request and response):
	clientTimestamp[8]
	requestType = 3: Sending server-computed gameState
	tick_id[1], gameObjectId[4], x[4], y[4], z[4] => buffer size = (gameObjects.size() * 4) + 1
	request_type = 5: ACK requests that require it: 
	request_id[2]

*/


//using std::chrono::operator""ms;

typedef std::chrono::duration<int, std::milli> milliseconds_type;

enum clientRequestTypes {
	userLogin=1, 
	clientGetRtt=2,
	sendClientState=4,
	clientAckRequest=5
};

enum serverResponseTypes {
	serverGetRtt=2,
	sendServerState=3,
	serverAckRequest=5, 
	sendServerDelta=6,
};


udp_server::udp_server(int port) {

	std::cout << "Will init UDP server on port: " << port << std::endl;
	try {
		this->server_endpoint = new udp::endpoint(udp::v4(), port);
		this->socket = new udp::socket(this->context, *this->server_endpoint);
	}
	catch (std::exception& err) {
		std::cerr << "Error while initializing UDP server: " << err.what() << std::endl;
	}
}

void udp_server::start_listening() {
	std::cout << "Listening to incoming connexions" << std::endl;
	try {
		f_orchestration = std::async(std::launch::async, orchestrate_object_movements, this);
		std::cout << "Started orchestration" << std::endl;
		while (true) {
			start_socket_receive();
			std::cout << "Stopped3: " << this->context.stopped() << std::endl;
			this->context.run();
			std::cout << "Every user left the server, waiting for upcoming connexion "<<this->context.stopped() << std::endl;
			this->context.restart();
			delete this->socket;
			this->socket = new udp::socket(this->context, *this->server_endpoint);
			std::cout << "Stopped2: " << this->context.stopped() << std::endl;

		}
	}
	catch (std::exception& error) {
		std::cerr << "Error while listening connexions: " << error.what() << std::endl;
	}
}

void udp_server::start_socket_receive() {
	this->socket->async_receive_from(boost::asio::buffer(this->receive_buffer), this->receive_endpoint, boost::bind(&udp_server::on_socket_receive, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

}

void udp_server::route_received_data(std::shared_ptr<udp::endpoint> current_receive_endpoint, unsigned char current_receive_buffer[RECEIVE_BUFFER_SIZE]) {
	//std::cout << "Running route_received_data with " << *current_receive_endpoint << std::endl;
	if (current_receive_buffer && current_receive_buffer[0]) {
		switch (current_receive_buffer[0])
		{
			case clientRequestTypes::userLogin:
			{
				//LogUser -> needs ACK (with gameobject id)
				User* foundUser = findUserByEndpoint(current_receive_endpoint);
				if (foundUser) {
					char ack_buffer[] = { serverResponseTypes::serverAckRequest, current_receive_buffer[1], current_receive_buffer[2], 0, 0, 0, 0 };
					// 5 (ACK identifier), receive_buffer[1-2] (request id), characte_uid (int -> 4bytes)
					int characterUid = foundUser->getCharacterUid();
					memcpy(&ack_buffer[3], &characterUid, 4);
					this->socket->send_to(boost::asio::buffer(ack_buffer), *current_receive_endpoint); //Ack login request; TODO: Make this a function
					//TODO: Handle reconnexion?
					//std::cout << "found user" << std::endl;
					std::cout << "Trying to re-log already existing user. Ignoring... "<<current_receive_endpoint<<std::endl;
				}
				else {
					std::cout << "A new user logged in and was added" << std::endl;
					//int* character_type = new int();
					//memcpy(character_type, &receive_buffer[2], 4);
					std::cout << "receive_endpoint_address outside: " << current_receive_endpoint << std::endl;
					logUser(current_receive_endpoint, current_receive_buffer, *this->socket);
					//delete character_type;
				}
			}
			break;
			case clientRequestTypes::clientGetRtt:
			{
				//std::cout << "Pause0" << std::endl;
				//auto serverTime = std::chrono::system_clock::now();
				//auto serverTime = std::chrono::high_resolution_clock::now();
				//auto serverTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(serverTime.time_since_epoch()).count();
				long long serverTimeMs = getTimestampMs();
				ClientBuffer* response = new ClientBuffer();
				unsigned char responseType = serverResponseTypes::serverGetRtt;
				response->pushBuffer(&responseType, sizeof(char));
				response->pushBuffer(&serverTimeMs, sizeof(serverTimeMs));
				response->pushBuffer(current_receive_buffer, 8 * sizeof(char), 1);

				//int serverTimeBuffer[1] = { serverTimeMs };
				std::vector<unsigned char> data = response->getBuffer();
				//this->socket->send_to(boost::asio::buffer({response->getBuffer()}), this->receive_endpoint);
				//std::cout << "Pause1" << std::endl;
				this->socket->send_to(boost::asio::buffer(data), *current_receive_endpoint);
				delete response;
				//std::cout << "Pause2" << std::endl;

			}
			break;
			case clientRequestTypes::sendClientState:
			{
				//std::cout << "Case 4" << std::endl;
				udp_server::handle_user_state(current_receive_endpoint, current_receive_buffer);

			}
			break;
			case clientRequestTypes::clientAckRequest:
			{
				User* userAcking = findUserByEndpoint(current_receive_endpoint);
				if (userAcking) {
					long long ackedTimestamp;
					memcpy(&ackedTimestamp, current_receive_buffer + 1, 8);
					userAcking->setLastAckedRequestTimestamp(ackedTimestamp);
				}
			}
			break;
			default:
				std::cout << "Received unknown request type (" <<(int) current_receive_buffer[0]<< ").ignoring..." << std::endl;
				break;
			}
	}
	else {
		std::cout << "Received empty buffer. Ignoring...";
	}

}

void udp_server::handle_user_state(std::shared_ptr<udp::endpoint> current_receive_endpoint, unsigned char current_receive_buffer[RECEIVE_BUFFER_SIZE]) {
	time_point_t serverTime = std::chrono::system_clock::now();

	User* userToHandle = findUserByEndpoint(current_receive_endpoint);
	if (!userToHandle) {
		//user is not reckognized, and his input is ignored
		return;
	}
	//get requestTime from 
	//TODO: see if requestTimeMs memory is freed when leaving this scope
	long long requestTimeMs;
	memcpy(&requestTimeMs, &current_receive_buffer[1], 8);
	time_point_t requestTime =std::chrono::system_clock::time_point(std::chrono::milliseconds(requestTimeMs));
	if (serverTime < requestTime) {
		std::cout << "Received a request dated in the future. Using current timestamp instead" << std::endl;
		requestTime = serverTime;
	}

	GameObjectPosition received_position = GameObjectPosition::deserialize(current_receive_buffer + 9);


	userToHandle->validateReceivedPosition(received_position.getValue(), requestTime);
	//TODO: implement per user mutex to prevent orchestrateObjectMovement from conflicting with setCharacterPosition
	userToHandle->setCharacterPosition(received_position);
	//TODO:store old positions of the  gameObject somewhere


}

void udp_server::on_socket_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
	
	std::shared_ptr<udp::endpoint> current_receive_endpoint = std::make_shared<udp::endpoint>(receive_endpoint);

	
	if (!error) {

		//DEBUG: Read receivedData
		// std::cout << "got data from endpoint: " << receive_endpoint << std::endl;
		//for (int i = 0; i < 3; i++) {
		//	//std::cout << "data " << i << ": " << (int)receive_buffer[i] << std::endl;
		//}
		//std::cout << "data " << (int)receive_buffer[0] << std::endl;
		//udp::endpoint current_receive_endpoint = receive_endpoint;
		unsigned char current_receive_buffer[RECEIVE_BUFFER_SIZE];
		memcpy(&current_receive_buffer, &receive_buffer, RECEIVE_BUFFER_SIZE);
		//udp::endpoint* current_receive_endpoint = new udp::endpoint(receive_endpoint);

		//std::stringstream endpoint_str;
		//endpoint_str << current_receive_endpoint;
		route_received_data(current_receive_endpoint, current_receive_buffer);
		udp_server::start_socket_receive();
	}
	else {
		std::cout << "Caught error in socket_receive" << std::endl;
		if (error.value() == 10061)
		{
			std::cout << "Player disconnected." << std::endl;
			findUserByEndpoint(current_receive_endpoint, true);

		}
	}

}

void udp_server::orchestrate_object_movements(udp_server* server) {
	auto tickStartTime = std::chrono::high_resolution_clock::now();
	//std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
	while (true) {
		GameObject::moveAllObjectsOneTick();
		//std::cout << "tick took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tickStartTime ).count() << " ms" << std::endl;
		sleep_until(tickStartTime + std::chrono::microseconds(1000000 / tick_rate));
		tickStartTime = std::chrono::high_resolution_clock::now();
		GameObject::registerAllHistories();

		//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(tickStartTime - begin).count() << "[µs]" << std::endl;
		//begin = std::chrono::high_resolution_clock::now();

		if (server->users.size()) {
			server->user_mutex.lock();
			for (User *oneUser : server->users) {
				std::vector<unsigned char> dataToSend = server->formatGameStateToSend(oneUser);
				oneUser->send_data(boost::asio::buffer(dataToSend));
			}
			server->user_mutex.unlock();
		}
		
		//std::cout << "one loop passed" << std::endl;
	}
}

User* udp_server::findUserByEndpoint(std::shared_ptr<udp::endpoint> tested_endpoint, bool erase){
	user_mutex.lock();
	for (size_t i = 0; i < this->users.size(); i++) {
		//std::cout << "Will compare: " << *tested_endpoint << " With " << this->users[i]->get_endpoint() << std::endl;
		if (this->users[i]->get_endpoint() == *tested_endpoint) {
			if (erase) {
				this->users.erase(this->users.begin() + i);
				user_mutex.unlock();
				return NULL;
			}
			user_mutex.unlock();
			return users[i];
		}
	}
	user_mutex.unlock();
	return NULL;

}

User* udp_server::findUserByEndpoint(std::shared_ptr<udp::endpoint> tested_endpoint) {
	return findUserByEndpoint(tested_endpoint, false);
}

void udp_server::logUser(std::shared_ptr<udp::endpoint> new_endpoint, unsigned char current_receive_buffer[RECEIVE_BUFFER_SIZE], udp::socket& socket) {
	user_mutex.lock();
	User* new_user = new User(new_endpoint, &socket);
	point_t start_position = point_t(0, 0, 0);
	new_user->setCharacterPosition(start_position);
	/*linestring_t new_trajectory = linestring_t();
	bg::append(new_trajectory, point_t(0, 0, 10));
	bg::append(new_trajectory, point_t(5, 0, 5));
	new_user->setCharacterTrajectory(new_trajectory);*/
	this->users.push_back(new_user);
	char ack_buffer[] = { 5, current_receive_buffer[1], current_receive_buffer[2], 0, 0, 0, 0 };
	int characterUid = new_user->getCharacterUid();
	memcpy(&ack_buffer[3], &characterUid, 4);
	this->socket->send_to(boost::asio::buffer(ack_buffer), *new_endpoint); //Ack login request
	//new_user->send_data(boost::asio::buffer(data));
	user_mutex.unlock();
}


std::vector<unsigned char> udp_server::formatGameStateToSend(User* userToSendStateTo) {

	long long serverTimeMs = getTimestampMs();
	ClientBuffer response = ClientBuffer();
	std::optional<ClientBuffer*> gameDeltaToSend = formatGameDeltaToSend(userToSendStateTo);
	
	//if the last user ack is not too old, send game delta, else, send whole game state.
	if (gameDeltaToSend.has_value()) {
		char* request_type = new char(serverResponseTypes::sendServerDelta);
		response.pushBuffer(request_type, sizeof(char));
		delete request_type; 
		response.pushBuffer(&serverTimeMs, sizeof(serverTimeMs));
		ClientBuffer* gameDeltaToSendValue = gameDeltaToSend.value();
		response.pushBuffer(gameDeltaToSendValue);
		delete gameDeltaToSendValue;
		return response.getBuffer();
	}
	std::cout << "State too old; sending whole gamestate" << std::endl;
	std::vector<GameObject*>* allGameObjects = GameObject::getGameObjects();


	
	char request_type = serverResponseTypes::sendServerState;
	response.pushBuffer(&request_type, sizeof(char));
	response.pushBuffer(&serverTimeMs, sizeof(serverTimeMs));

	//Data format: requestType[1] = 3, server_timestamp[8], gameObjectId[4], x[4], y[4], z[4], actionId[4], actionFrame[4] => buffer size = (gameObjects.size() * 24) + 9
	for (GameObject* oneGameObject : *allGameObjects) {
		ClientBuffer oneObjectBuffer = oneGameObject->toClientBuffer();
		response.pushBuffer(&oneObjectBuffer);
	}
	return response.getBuffer();
}

//Data format <user whole state>, <deltas>
//user whole state format: gameObjectId[4], x[4], y[4], z[4], actionId[4], actionFrame[4]
// delta format: changeType[1] (create, update, delete),(dataType_position[1],  position[12]),(dataType_actionId[1], actionId[4], actionFrame[4]) 0xff
// Note: 0xff means "end of this gameObject. To prevent colision, dataId 0xff is reserved.
std::optional<ClientBuffer*> udp_server::formatGameDeltaToSend(User* userToSendStateTo) {

	ClientBuffer* bufferToReturn = new ClientBuffer();
	std::vector<GameObject*>* allGameObjects = GameObject::getGameObjects();
	time_point_t lastAckedRequest = userToSendStateTo->getLastAckedRequestTimestamp();
	ClientBuffer userStateBuffer = userToSendStateTo->getCharacter()->toClientBuffer();
	bufferToReturn->pushBuffer(&userStateBuffer);

	for (GameObject* oneGameObject : *allGameObjects) {
		if (oneGameObject->getUid() == userToSendStateTo->getCharacterUid()) {
			continue;
		}
		std::optional<StateDelta> oneStateDelta = oneGameObject->getHistory()->getDeltaSince(lastAckedRequest);
		unsigned int oneUid = oneGameObject->getUid();
		if (!oneStateDelta.has_value()) {
			delete bufferToReturn;
			return std::nullopt;
		}

		StateDelta oneStateDeltaValue = oneStateDelta.value();
		if (oneStateDeltaValue.changeType == ChangeType::none) {
			continue;
		}
		ClientBuffer oneStateDeltaBuffer = oneStateDeltaValue.toClientBuffer();
		bufferToReturn->pushBuffer(&oneUid, sizeof(unsigned int));
		bufferToReturn->pushBuffer(&oneStateDeltaBuffer);
	}
	return bufferToReturn;
}