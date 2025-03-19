#pragma once
#include "GameObjectProperty.h"
class GameObjectSkin : public GameObjectProperty {
private:
    unsigned int skinId;
public:
    static const int DataId = 3;
    static GameObjectSkin deserialize(unsigned char* skinIdBytes);

    virtual void serializeInBuffer(ClientBuffer& buffer, bool includePropertyType = false) override;

    GameObjectSkin(unsigned int skinId) : skinId(skinId) {};
    unsigned int getValue();
    bool operator==(const GameObjectSkin& other) const;
    bool operator!=(const GameObjectSkin& other) const;

};