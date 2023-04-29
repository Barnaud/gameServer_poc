#include "ClientBuffer.h"
#include <iostream>
#include <string>


void ClientBuffer::pushBuffer(void* eltToPush, unsigned int len, unsigned int skip) {
	for (unsigned int i = skip; i < len + skip; i++) {
		unsigned char* tmpByte = (unsigned char*)eltToPush +i;
		this->internalBuffer.push_back(*tmpByte);
	}

}

void ClientBuffer::pushPoint(point_t& const pointToPush) {
	float coordinates[3];

	coordinates[0] = bg::get<0>(pointToPush);
	coordinates[1] = bg::get<1>(pointToPush);
	coordinates[2] = bg::get<2>(pointToPush);

	for (int i = 0; i < 3; i++) {
		pushBuffer(&coordinates[i], sizeof(float));
	}
}

std::vector<unsigned char> ClientBuffer::getBuffer() {
	return this->internalBuffer;
}