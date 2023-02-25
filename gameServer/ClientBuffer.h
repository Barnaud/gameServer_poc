#pragma once
#include <vector>

class ClientBuffer {

	private:
		std::vector<unsigned char> internalBuffer;

	public:
		//ClientBuffer(void* data,int length);
		void pushBuffer(void* eltToPush, unsigned int len, unsigned int skip = 0);
		std::vector<unsigned char> getBuffer();
};