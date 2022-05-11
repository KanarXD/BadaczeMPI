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

int ProcessData::incrementClock() {
    std::lock_guard _{clockMutex};
    return ++clock;
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

ProcessState ProcessData::getProcessState() const {
    return processState;
}

void ProcessData::setProcessState(ProcessState processState) {
    ProcessData::processState = processState;
}
