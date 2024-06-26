#include "GameObject.h"
#include "Constants.h"

using namespace std::chrono_literals;

std::vector<GameObject*> GameObject::gameObjects;
unsigned int GameObject::max_uid = 0;

GameObject::GameObject() {
	std::cout << "Creating GameObject 1" << std::endl;
	uid = ++max_uid;
	std::cout << "Newly created gameObject uid is: " << uid << std::endl;
	position = point_t(0, 0, 0);
	trajectory = linestring_t();
	default_speed = 1;
	gameObjects.push_back(this);
	std::cout << "Gameobjects size: " << gameObjects.size();

	//GameObject(point_t(0, 0, 0), linestring_t());
}
GameObject::GameObject(point_t position_a) : position(position_a)
{
	std::cout << "Creating GameObject 2" << std::endl;
	uid = ++max_uid;
	trajectory = linestring_t();
	default_speed = 1;
	gameObjects.push_back(this);
	//GameObject(position_a, linestring_t());

}
GameObject::GameObject(point_t position_a, linestring_t trajectory_a) : position(position_a), trajectory(trajectory_a)
{
	std::cout << "Creating GameObject 3" << std::endl;
	uid = ++max_uid;
	default_speed = 1;
	gameObjects.push_back(this);
}
GameObject::~GameObject() {
	//std::cout << "Deleting gameObject" << std::endl;
	for (auto it = GameObject::gameObjects.begin(); it != GameObject::gameObjects.end(); it++) {
		if (*it == this) {
			gameObjects.erase(it);
			return;
		}
	}
}


void GameObject::setPosition(GameObjectPosition new_position) {
	this->position = new_position;
}

GameObjectPosition GameObject::getPosition() {
	return position;
}

GameObjectState GameObject::getState() {
	GameObjectState stateToReturn = GameObjectState(position, action);
	stateToReturn.position = position;
	return stateToReturn;
}

void GameObject::setTrajectory(linestring_t new_trajectory) {
	trajectory = new_trajectory;
}

linestring_t GameObject::getTrajectory() {
	return trajectory;
}

unsigned int GameObject::getUid() {
	return uid;
}

ClientBuffer GameObject::toClientBuffer() {

	ClientBuffer bufferToReturn = ClientBuffer();
	bufferToReturn.pushBuffer(&uid, sizeof(uid));
	position.serializeInBuffer(bufferToReturn);
	action.serializeInBuffer(bufferToReturn);
	
	return bufferToReturn;
}

GameObjectStateHistory* GameObject::getHistory() {
	return &history;
}

void GameObject::moveOneTick() {
	return moveOneTick(default_speed);
}

void GameObject::moveAllObjectsOneTick() {
	for (GameObject* oneGameObject : GameObject::gameObjects) {
		oneGameObject->moveOneTick();
	}
}

void GameObject::registerAllHistories() {
	time_point_t now_timestamp = std::chrono::system_clock::now();
	std::chrono::seconds oneSecond(1);
	time_point_t oneSecondAgo = now_timestamp - oneSecond;
	for (GameObject* oneGameObject : GameObject::gameObjects) {
		GameObjectState currentState = oneGameObject->getState();
		currentState.timestamp = now_timestamp;
		oneGameObject->history.registerStateToHistory(currentState);
		/*auto myDelta = oneGameObject->history.getDeltaSince(oneSecondAgo);
		if (myDelta != std::nullopt && myDelta.value().changeType == changeType::updated) {
			std::cout << "state updated" << std::endl;
		}*/
		//std::cout << "aa" << std::endl;
	}
}
std::vector<GameObject*>* GameObject::getGameObjects() {
	return &gameObjects;

}


void GameObject::moveOneTick(float custom_speed) {
	if (custom_speed <= 0 || !trajectory.size()) {
		return;
	}

	float moveDistance = custom_speed / tick_rate;
	//std::cout << "Move distance: " << moveDistance << std::endl;
	const bool willReachNextPoint = bg::distance(trajectory[0], position.getValue()) < moveDistance;

	if ( willReachNextPoint && trajectory.size() == 1) {
		position = trajectory[0];
		bg::clear(trajectory);
		return;
	}
	else if (willReachNextPoint) {
		trajectoryPullFront();
	}
	point_t& next_dest = trajectory[0];
	point_t next_position = next_dest;
	bg::subtract_point(next_position, position.getValue());
	bg::multiply_value(next_position, ((1.0f / bg::distance(next_dest, position.getValue()) * moveDistance)));
	bg::add_point(next_position, position.getValue());
	position = next_position;

}

void GameObject::trajectoryPullFront() {
	std::vector<point_t> new_points;

	for (size_t i = 0; i < trajectory.size(); i++) {
		if (i != 0) {
			new_points.push_back(trajectory[i]);
		}
	}
	bg::assign_points(trajectory,  new_points);
}