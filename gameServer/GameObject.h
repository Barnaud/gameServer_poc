#pragma once
#include <vector>
#include "geometry_typedef.h"


class GameObject
{

public:
	GameObject();
	GameObject(point_t position_a);
	GameObject(point_t position_a, linestring_t trajectory_a);
	~GameObject();

	void setPosition(point_t new_position);
	point_t getPosition();

	void setTrajectory(linestring_t new_trajectory);
	linestring_t getTrajectory();

	unsigned int getUid();
	void moveOneTick();
	void moveOneTick(float custom_speed);
	static std::vector<GameObject*>* getGameObjects();
	static void moveAllObjectsOneTick();


private:
	point_t position = point_t(0, 0);
	linestring_t trajectory;
	float default_speed;
	unsigned uid;

	void trajectoryPullFront();

	static unsigned int max_uid;
	static std::vector<GameObject*> gameObjects;
};