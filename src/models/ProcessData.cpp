#include "ProcessData.h"

ProcessData::ProcessData(int processId, int groupSize, const Settings &settings) :
        processId(processId),
        groupList(std::vector<std::vector<int>>(groupSize)),
        settings(settings) {
}

int ProcessData::getClock() const {
    return clock;
}

int ProcessData::getProcessId() const {
    return processId;
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

void ProcessData::setProcessState(ProcessState state) {
    ProcessData::processState = state;
}

std::mutex &ProcessData::getWaitResourceMutex() {
    return waitResourceMutex;
}

int ProcessData::getAckCount() const {
    return ackCount;
}

void ProcessData::setAckCount(int count) {
    ProcessData::ackCount = count;
}
