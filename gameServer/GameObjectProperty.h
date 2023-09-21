#pragma once
#include "ClientBuffer.h"
class GameObjectProperty
{
	public: 
		virtual void  serializeInBuffer(ClientBuffer& buffer, bool includePropertyType = false) = 0;
};

