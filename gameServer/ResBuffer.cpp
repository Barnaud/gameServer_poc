#include "ResBuffer.h"
#include <iostream>
#include <string>

void ResBuffer::pushBuffer(void* eltToPush, unsigned int len, unsigned int skip) {
	for (unsigned int i = skip; i < len + skip; i++) {
		unsigned char* tmpByte = (unsigned char*)eltToPush +i;
		this->internalBuffer.push_back(*tmpByte);
	}

}

std::vector<unsigned char> ResBuffer::getBuffer() {
	return this->internalBuffer;
}