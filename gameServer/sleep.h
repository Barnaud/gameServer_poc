#pragma once

#include<chrono>
#include<thread>

void sleep_until(std::chrono::time_point<std::chrono::steady_clock> tp);