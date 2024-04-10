#include "GameObjectPosition.h"


point_t& GameObjectPosition::getValue() {
    return position;
}

//TODO: use (smart) pointers to prevent copying position twice on each deserialization
GameObjectPosition GameObjectPosition::deserialize(unsigned char* positionBytes) {
    float x, y, z, r;
    memcpy(&x, positionBytes, 4);
    memcpy(&y, positionBytes + 4, 4);
    memcpy(&z, positionBytes + 8, 4);
    //memcpy(&r, &current_receive_buffer[12], 4);

    //TODO: check that provided position is not too far from the previous one, that walls are not being crossed

    point_t received_position = point_t(x, y, z);
    return GameObjectPosition(received_position);
}

bool GameObjectPosition::operator==(const GameObjectPosition& other) const {
    return bg::equals(position, other.position);
}

bool GameObjectPosition::operator!=(const GameObjectPosition& other) const {
    return !operator==(other);
}

void GameObjectPosition::serializeInBuffer(ClientBuffer& buffer, bool includePropertyType) {
    if (includePropertyType) {
        char positionDataId = GameObjectPosition::DataId;
        buffer.pushBuffer(&positionDataId, sizeof(positionDataId));
    }
    buffer.pushPoint(position);
}