#pragma once
#include "ClientBuffer.h"

/*
* To add a property to gameObjects: 
* - Create a class that extends GameObjectProperties (and with the required static functions and the equal operators)
* - Add the class in GameObjects (with the right getter/setters if required)
* - Add the class to GameObjectState
* - Add a call to detectAndSaveParameterUpdateInDelta in GameObjectStateHistory
* - Add the property in "GameObjects::toClientBuffer" 
*/
class GameObjectProperty
{
	public: 
		virtual void  serializeInBuffer(ClientBuffer& buffer, bool includePropertyType = false) = 0;
};

