#pragma once
#include <vector>
#include "geometry_typedef.h"


class ClientBuffer {

	private:
		std::vector<unsigned char> internalBuffer;

	public:
		//ClientBuffer(void* data,int length);
		void pushBuffer(void* eltToPush, unsigned int len, unsigned int skip = 0);
		void pushBuffer(ClientBuffer* bufferToPush);
		void pushPoint(const point_t& pointToPush);
		std::vector<unsigned char> getBuffer();
};