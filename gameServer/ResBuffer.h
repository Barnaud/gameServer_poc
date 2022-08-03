#pragma once
#include <vector>

class ResBuffer {

	private:
		std::vector<unsigned char> internalBuffer;

	public:
		void pushBuffer(void* eltToPush, unsigned int len, unsigned int skip = 0);
		std::vector<unsigned char> getBuffer();
};