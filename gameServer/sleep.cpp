#include "sleep.h"


using std::chrono::operator""ms;

typedef std::chrono::duration<int, std::milli> milliseconds_type;

//valid from 10ms; not less
void sleep_until(std::chrono::high_resolution_clock::time_point tp) {

	std::this_thread::sleep_for(1ms);
	bool isTimerFinished = false;

	do {
		std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
		isTimerFinished= now >= tp;
	} while (!isTimerFinished);



}