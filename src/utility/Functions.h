#pragma once

#include <thread>
#include <random>
#include <chrono>
#include <ctime>

#define MIN_SLEEP_TIME 200
#define MAX_SLEEP_TIME 1000

namespace Functions {
    static std::mt19937 rng(0);

    static void initRNG() {
        duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    static unsigned long getRandomNumber() {
        return rng();
    }

    static void sleep() {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(MIN_SLEEP_TIME + getRandomNumber() % (MAX_SLEEP_TIME - MIN_SLEEP_TIME)));
    }

    static bool makeDecision(int percentChance) {
        return percentChance > getRandomNumber() % 100;
    }



}
