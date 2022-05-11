#pragma once

#include <ostream>

struct Settings {
    int groupSize;
    int UNRCount;
    int GroupCount;
    int processCount;

    friend std::ostream &operator<<(std::ostream &os, const Settings &settings) {
        os << "groupSize: " << settings.groupSize << " UNRCount: " << settings.UNRCount << " GroupCount: "
           << settings.GroupCount << " processCount: " << settings.processCount;
        return os;
    }
};
