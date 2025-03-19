#include "GameObjectSkin.h"

GameObjectSkin GameObjectSkin::deserialize(unsigned char* skinIdBytes) {
    unsigned int returnedSkinId;
    memcpy(&returnedSkinId, skinIdBytes, sizeof(unsigned int));
    return GameObjectSkin(returnedSkinId);
}

void GameObjectSkin::serializeInBuffer(ClientBuffer &buffer, bool includePropertyType) {
    if (includePropertyType) {
        char skinDataId = GameObjectSkin::DataId;
        buffer.pushBuffer(&skinDataId, sizeof(skinDataId));
    }
    buffer.pushBuffer(&skinId, sizeof(unsigned int));
}

unsigned int GameObjectSkin::getValue() {
    return skinId;

}
bool GameObjectSkin::operator==(const GameObjectSkin& other) const{
    return skinId == other.skinId;
}
bool GameObjectSkin::operator!=(const GameObjectSkin& other) const {
    return skinId != other.skinId;
}