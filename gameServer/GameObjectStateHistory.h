#pragma once
#include <iostream>
#include <queue>
#include <optional>
#include "geometry_typedef.h"
#include "chrono_typedef.h"
#include "ClientBuffer.h"

#define savedStatesCount 255

enum class ChangeType {
	none=0,
	created=1,
	updated=2,
	deleted=3,
};

enum class DataId {
	position=1,
	actionId=2,
};

struct GameObjectState  {
	time_point_t timestamp;
	bool isDestroyed=false;
	point_t position;
	int actionId=0;
	int action_frame=0;

};

struct StateDelta {
	ChangeType changeType = ChangeType::none;
	std::optional<point_t> newPosition = std::nullopt;
	std::optional<int>newActionId = std::nullopt;
	int newActionFrame=0;

	ClientBuffer toClientBuffer();
};

class GameObjectStateHistory {
private:
	//a priority queue may be safer, but the risk of inserting in the wrong order is weak
	std::deque<GameObjectState> states;

public: 
	std::deque<GameObjectState> getStates() const;
	void registerStateToHistory(GameObjectState stateToEnqueue);
	std::optional<StateDelta> getDeltaSince(time_point_t timestamp);
	time_point_t getLatestTimestamp();
	time_point_t getOldestTimestamp();
};