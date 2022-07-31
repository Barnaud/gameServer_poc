#include "sleep.h"


using std::chrono::operator""ms;

typedef std::chrono::duration<int, std::milli> milliseconds_type;

//valid from 10ms; not less
void sleep_until(std::chrono::time_point<std::chrono::steady_clock> tp) {

	std::this_thread::sleep_for(1ms);
	bool isTimerFinished = false;

	do {
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		isTimerFinished= now >= tp;
	} while (!isTimerFinished);



}