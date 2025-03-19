#pragma once
#include <iostream>
#include <queue>
#include <optional>
#include "geometry_typedef.h"
#include "chrono_typedef.h"
#include "ClientBuffer.h"
#include "GameObjectPosition.h"
#include "GameObjectAction.h"
#include "GameObjectSkin.h"

#define savedStatesCount 255

enum class ChangeType {
	none=0,
	created=1,
	updated=2,
	deleted=3,
};

//TODO: Delete
enum class DataId {
	position=1,
	actionId=2,
};

struct GameObjectState  {
	GameObjectState(const GameObjectPosition position_a, const GameObjectAction action_a, const GameObjectSkin skin_a): position(position_a), action(action_a), skin(skin_a) {
	}
	time_point_t timestamp;
	bool isDestroyed=false;
	GameObjectPosition position;
	GameObjectAction action;
	GameObjectSkin skin;
};

struct StateDelta {
	ChangeType changeType = ChangeType::none;
	std::optional<GameObjectPosition> newPosition = std::nullopt;
	std::optional<GameObjectAction> newAction = std::nullopt;
	std::optional<GameObjectSkin> newSkin = std::nullopt;

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

	//template <typename T>
	//void detectAndSaveParameterUpdateInDelta(const T& previousState, const T& currentState, StateDelta& deltaToUpdate, std::optional<T>& stateToSave);
};