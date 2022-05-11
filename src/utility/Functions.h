#pragma once

#include <thread>

#define MIN_SLEEP_TIME 200
#define MAX_SLEEP_TIME 2000

class Functions {
public:

    static void sleep() {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(MIN_SLEEP_TIME + rand() % (MAX_SLEEP_TIME - MIN_SLEEP_TIME)));
    }

    static bool makeDecision(int percentChance) {
        return rand() % 100 < percentChance;
    }

};
