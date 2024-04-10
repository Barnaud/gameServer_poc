#include "GameObjectAction.h"


unsigned int& GameObjectAction::getActionId() {
	return actionId;
}

unsigned int& GameObjectAction::getActionFrame() {
	return actionFrame;
}

bool GameObjectAction::operator==(const GameObjectAction& other) const {
	return actionId == other.actionId && actionFrame == other.actionFrame;
}

bool GameObjectAction::operator!=(const GameObjectAction& other) const {
	return !operator==(other);
}

void GameObjectAction::serializeInBuffer(ClientBuffer& buffer, bool includePropertyType) {
	if (includePropertyType) {
		char actionDataId = GameObjectAction::DataId;
		buffer.pushBuffer(&actionDataId, sizeof actionDataId);
	}
	buffer.pushBuffer(&actionId, sizeof(actionId));
	buffer.pushBuffer(&actionFrame, sizeof(actionFrame));
}