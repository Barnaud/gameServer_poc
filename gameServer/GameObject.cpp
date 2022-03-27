#include "GameObject.h"
#include "Constants.h"

std::vector<GameObject*> GameObject::gameObjects;

GameObject::GameObject() {
	gameObjects.push_back(this);
}
GameObject::GameObject(point_t position_a) : position(position_a)
{
	gameObjects.push_back(this);
}
GameObject::GameObject(point_t position_a, linestring_t trajectory_a) : position(position_a), trajectory(trajectory_a)
{
	gameObjects.push_back(this);
}
GameObject::~GameObject() {
	std::cout << "Deleting gameObject" << std::endl;
	for (auto it = GameObject::gameObjects.begin(); it != GameObject::gameObjects.end(); it++) {
		if (*it == this) {
			gameObjects.erase(it);
			return;
		}
	}
}


void GameObject::setPosition(point_t new_position) {
	this->position = new_position;
}

point_t GameObject::getPosition() {

	return position;
}

void GameObject::moveOneTick() {
	return moveOneTick(default_speed);
}

void GameObject::moveAllObjectsOneTick()
{
	for (GameObject* oneGameObject : GameObject::gameObjects) {
		oneGameObject->moveOneTick();
	}
}
std::vector<GameObject*>* GameObject::getGameObjects() {
	return &gameObjects;

}

void GameObject::moveOneTick(float custom_speed) {
	if (custom_speed <= 0 || !trajectory.size()) {
		return;
	}

	point_t& next_dest = trajectory[0];
	point_t next_position = next_dest;
	bg::subtract_point(next_position, position);
	bg::multiply_value(next_position, (1/bg::distance(next_dest, position)) * custom_speed *(1/tick_rate));
	bg::add_point(next_position, position);
	position = next_position;
}