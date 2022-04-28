#pragma once

#include <ostream>
#include "vector"


struct Settings {
    int processCount;
    int groupSize;
    int UNRCount;
    int GroupCount;

    friend std::ostream &operator<<(std::ostream &os, const Settings &settings) {
        os << "processCount: " << settings.processCount << " groupSize: " << settings.groupSize << " UNRCount: "
           << settings.UNRCount << " GroupCount: " << settings.GroupCount;
        return os;
    }
};

struct ProcessData {
    int processId;
    int clock;
    int processCount;
    std::vector<int> *groupList;

    ProcessData(int groupSize) :
            clock(0),
            groupList(
                    new std::vector<int>[groupSize]) {}

    friend std::ostream &operator<<(std::ostream &os, const ProcessData &data) {
        os << "processId: " << data.processId << " clock: " << data.clock << " processCount: " << data.processCount
           << " groupList: " << data.groupList;
        return os;
    }
};
