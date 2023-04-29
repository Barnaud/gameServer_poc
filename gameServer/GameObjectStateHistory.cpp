#pragma once
#include "GameObjectStateHistory.h"

std::deque<GameObjectState> GameObjectStateHistory::getStates() const {
	return states;
}

void GameObjectStateHistory::enqueue(GameObjectState stateToEnqueue) {
	stateToEnqueue.timestamp = std::chrono::system_clock::now();
	if (states.front().timestamp < stateToEnqueue.timestamp) {
		states.push_front(stateToEnqueue);
		if (states.size() > savedStatesCount) {
			states.pop_back();
		}
	}
}

//nullopt = too old timestamp requested
//deltatype none = no changes
std::optional<StateDelta> GameObjectStateHistory::getDeltaSince(time_point_t origin_timestamp) {
	if (states.empty() || (origin_timestamp < getOldestTimestamp() && states.size() >= savedStatesCount)) {
		return std::nullopt;
	}

	StateDelta stateDeltaToReturn = StateDelta();

	if (origin_timestamp >= getLatestTimestamp()) {
		return stateDeltaToReturn;
	}
		

	int observedStateIndex = 0;
	while (states[observedStateIndex].timestamp > origin_timestamp && observedStateIndex < states.size() - 1) {
		GameObjectState& thisTickState = states[observedStateIndex];
		GameObjectState& previousTickState = states[observedStateIndex + 1];
		if (thisTickState.isDestroyed && !previousTickState.isDestroyed) {
			stateDeltaToReturn.changeType = changeType::deleted;
			return stateDeltaToReturn;
		}
		if (!thisTickState.isDestroyed) {
			if (stateDeltaToReturn.newPosition != std::nullopt && !bg::equals(thisTickState.position, previousTickState.position)) {
				stateDeltaToReturn.newPosition = thisTickState.position;
				stateDeltaToReturn.changeType = changeType::updated;
			}
			if (stateDeltaToReturn.newActionId != std::nullopt && thisTickState.actionId != previousTickState.actionId) {
				stateDeltaToReturn.newActionId = thisTickState.actionId;
				stateDeltaToReturn.newActionFrame = thisTickState.actionId;
				stateDeltaToReturn.changeType = changeType::updated;
			}

		}

	}

	if (origin_timestamp < getOldestTimestamp() && states.size() < savedStatesCount) {
		stateDeltaToReturn.changeType = changeType::created;
		if (stateDeltaToReturn.newPosition = std::nullopt) {
			stateDeltaToReturn.newPosition = states[0].position;
		}
		if (stateDeltaToReturn.newActionId = std::nullopt) {
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

ClientBuffer StateDelta::toClientBuffer() {
	ClientBuffer bufferToReturn = ClientBuffer();
	char charChangeType = (char)changeType;
	bufferToReturn.pushBuffer(&changeType, sizeof(char));
	if (newPosition != std::nullopt) {
		char charPositionDataId = (char)dataId::position;
		bufferToReturn.pushBuffer(&charPositionDataId, sizeof(char));
		bufferToReturn.pushPoint(newPosition.value());
	}
	if (newActionId != std::nullopt) {
		char charActionIdDataId = (char)dataId::actionId;
		bufferToReturn.pushBuffer(&charActionIdDataId, sizeof(char));
		bufferToReturn.pushBuffer(&(newActionId.value()), sizeof(int));
		bufferToReturn.pushBuffer(&newActionFrame, sizeof(int));
	}
}