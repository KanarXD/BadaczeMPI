#pragma once

#include <ostream>

struct Message {
    int processId;
    int clock;

    friend std::ostream &operator<<(std::ostream &os, const Message &message) {
        os << "processId: " << message.processId << " clock: " << message.clock;
        return os;
    }
};