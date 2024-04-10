#pragma once
#include "GameObjectProperty.h"
#include "geometry_typedef.h"
class GameObjectPosition :
    public GameObjectProperty
{
private: 
    point_t position;

public: 
    static const char DataId = 1;

    static GameObjectPosition deserialize(unsigned char* positionBytes);

    GameObjectPosition(point_t position_a) : position(position_a) {

    };
    point_t& getValue();
    bool operator==(const GameObjectPosition& other) const;
    bool operator!=(const GameObjectPosition& other) const;
    virtual void serializeInBuffer(ClientBuffer& buffer, bool includePropertyType = false) override;

    
};

