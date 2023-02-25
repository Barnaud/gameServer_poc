#pragma once

#include<chrono>
#include<thread>

void sleep_until(std::chrono::high_resolution_clock::time_point tp);

long long getTimestampMs();
