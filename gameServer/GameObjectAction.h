#pragma once
#include "GameObjectProperty.h"
class GameObjectAction :
    public GameObjectProperty
{
private:
    unsigned int actionId;
    unsigned int actionFrame;

public: 
    
    static const int DataId = 2; 

    GameObjectAction(unsigned int actionId_a, unsigned int ActionFrame_a) : actionId(actionId_a), actionFrame(ActionFrame_a) {

    };
    virtual void  serializeInBuffer(ClientBuffer& buffer, bool includePropertyType = false);
    bool operator==(GameObjectAction const& other) const ;
    bool operator!= (GameObjectAction const& other) const;

    unsigned int& getActionId();
    unsigned int& getActionFrame();

};

