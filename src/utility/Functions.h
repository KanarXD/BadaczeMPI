#pragma once

#include <thread>
#include <random>

#define MIN_SLEEP_TIME 500
#define MAX_SLEEP_TIME 1500

namespace Functions {

    static unsigned long getRandomNumber() {
        long time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
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
