#pragma once
#include <vector>
#include "geometry_typedef.h"
#include "GameObjectStateHistory.h"


class GameObject
{

public:
	GameObject();
	GameObject(point_t position_a);
	GameObject(point_t position_a, linestring_t trajectory_a);
	~GameObject();

	void setPosition(point_t new_position);
	point_t getPosition();

	GameObjectState getState();

	void setTrajectory(linestring_t new_trajectory);
	linestring_t getTrajectory();

	unsigned int getUid();
	void moveOneTick();
	void moveOneTick(float custom_speed);

	ClientBuffer toClientBuffer();
	GameObjectStateHistory* getHistory();
	static std::vector<GameObject*>* getGameObjects();
	static void moveAllObjectsOneTick();
	static void registerAllHistories();


private:
	point_t position = point_t(0, 0);
	linestring_t trajectory;
	float default_speed;
	unsigned int uid;

	int actionId = 0;
	int actionFrame = 0;

	void trajectoryPullFront();

	GameObjectStateHistory history;

	static unsigned int max_uid;
	static std::vector<GameObject*> gameObjects;
};