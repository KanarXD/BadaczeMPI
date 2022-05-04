#include "ProcessData.h"

ProcessData::ProcessData(int processId, int processCount, int groupSize, const Settings &settings) :
        processId(processId),
        processCount(processCount),
        groupList(std::vector<std::vector<int>>(groupSize)),
        settings(settings) {

}

int ProcessData::getClock() const {
    return clock;
}

int ProcessData::getProcessId() const {
    return processId;
}

int ProcessData::getProcessCount() const {
    return processCount;
}

void ProcessData::increaseClock() {
    std::lock_guard _{clockMutex};
    clock++;
}

void ProcessData::setClock(int newClock) {
    std::lock_guard _{clockMutex};
    if (clock < newClock) {
        clock = newClock + 1;
    }
}

void ProcessData::addProcessToGroup(int group, int process) {
    std::lock_guard _{groupListMutex};
    groupList[group].push_back(process);
}

const Settings &ProcessData::getSettings() const {
    return settings;
}
