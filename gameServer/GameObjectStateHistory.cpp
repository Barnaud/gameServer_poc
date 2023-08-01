#pragma once
#include "GameObjectStateHistory.h"

std::deque<GameObjectState> GameObjectStateHistory::getStates() const {
	return states;
}

void GameObjectStateHistory::registerStateToHistory(GameObjectState stateToEnqueue) {
	//std::cout << "states size: " << states.size() << std::endl;
	stateToEnqueue.timestamp = std::chrono::system_clock::now();
	if (states.empty() || getLatestTimestamp() < stateToEnqueue.timestamp) {
		states.push_front(stateToEnqueue);
		if (states.size() > savedStatesCount) {
			states.pop_back();
		}
	}
}

//nullopt = too old timestamp requested -> send whole gamestate
//deltatype none = no changes
std::optional<StateDelta> GameObjectStateHistory::getDeltaSince(time_point_t origin_timestamp) {
	if (states.empty() || (origin_timestamp < getOldestTimestamp() && states.size() >= savedStatesCount)) {
		return std::nullopt;
	}

	StateDelta stateDeltaToReturn = StateDelta();

	if (origin_timestamp >= getLatestTimestamp()) {
		return stateDeltaToReturn;
	}
		
	//state 0 (front) is the newest state
	int observedStateIndex = 0;
	while (states[observedStateIndex].timestamp > origin_timestamp && observedStateIndex < states.size() - 1) {
		GameObjectState& thisTickState = states[observedStateIndex];
		GameObjectState& previousTickState = states[observedStateIndex + 1];
		if (thisTickState.isDestroyed && !previousTickState.isDestroyed) {
			stateDeltaToReturn.changeType = changeType::deleted;
			return stateDeltaToReturn;
		}
		if (!thisTickState.isDestroyed) {
			if (stateDeltaToReturn.newPosition == std::nullopt && !bg::equals(thisTickState.position, previousTickState.position)) {
				stateDeltaToReturn.newPosition = thisTickState.position;
				stateDeltaToReturn.changeType = changeType::updated;
			}
			if (stateDeltaToReturn.newActionId == std::nullopt && thisTickState.actionId != previousTickState.actionId) {
				stateDeltaToReturn.newActionId = thisTickState.actionId;
				stateDeltaToReturn.newActionFrame = thisTickState.actionId;
				stateDeltaToReturn.changeType = changeType::updated;
			}

		}
		observedStateIndex++;

	}

	if (origin_timestamp < getOldestTimestamp() && states.size() < savedStatesCount) {
		stateDeltaToReturn.changeType = changeType::created;
		if (stateDeltaToReturn.newPosition == std::nullopt) {
			stateDeltaToReturn.newPosition = states[0].position;
		}
		if (stateDeltaToReturn.newActionId == std::nullopt) {
			stateDeltaToReturn.newActionId = states[0].actionId;
			stateDeltaToReturn.newActionFrame = states[0].actionId;
		}
	}

	return stateDeltaToReturn;
	


}

time_point_t GameObjectStateHistory::getLatestTimestamp() {
	if (states.empty()) {
		return time_point_t();
	}
	return states.front().timestamp;
}

time_point_t GameObjectStateHistory::getOldestTimestamp() {
	if (states.empty()) {
		return std::chrono::system_clock::now();
	}
	return states.back().timestamp;

}

/*
* Format of client buffer (for one gameObject) :
* bits between parentheses means optional
* changeType[1] (create, update, delete),(dataType_position[1],  position[12]),(dataType_actionId[1], actionId[4], actionFrame[4]) 0xff
* Note: 0xff means "end of this gameObject. To prevent colision, dataId 0xff is reserved.
* Don't call this function if change type is null.
*/

ClientBuffer StateDelta::toClientBuffer() {
	ClientBuffer bufferToReturn = ClientBuffer();
	char charChangeType = (unsigned char)changeType;
	bufferToReturn.pushBuffer(&changeType, sizeof(char));
	if (newPosition != std::nullopt) {
		char charPositionDataId = (unsigned char)dataId::position;
		bufferToReturn.pushBuffer(&charPositionDataId, sizeof(char));
		bufferToReturn.pushPoint(newPosition.value());
	}
	if (newActionId != std::nullopt) {
		char charActionIdDataId = (unsigned char)dataId::actionId;
		bufferToReturn.pushBuffer(&charActionIdDataId, sizeof(char));
		bufferToReturn.pushBuffer(&(newActionId.value()), sizeof(int));
		bufferToReturn.pushBuffer(&newActionFrame, sizeof(int));
	}
	char endOfObjectDelimiter = 0xff;
	bufferToReturn.pushBuffer(&endOfObjectDelimiter, sizeof(char));	

	return bufferToReturn;
}