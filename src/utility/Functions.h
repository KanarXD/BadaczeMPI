#pragma once

#include <thread>
#include <random>

#define MIN_SLEEP_TIME 200
#define MAX_SLEEP_TIME 1000

namespace Functions {

    static unsigned long getRandomNumber() {
        long time = duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        std::mt19937 rng(time);
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
