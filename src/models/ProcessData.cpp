#include "ProcessData.h"
#include "../utility/Log.h"

ProcessData::ProcessData(int processId, const Settings &settings) :
        processId(processId),
        groupList(std::vector<std::set<int>>(settings.GroupCount)),
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
    groupList[group].insert(process);
}

void ProcessData::removeProcessFromGroup(int group, int process) {
    std::lock_guard _{groupListMutex};
    LOGDEBUG("removeProcessFromGroup process: ", process, ", group: ", group);
    groupList[group].erase(process);
}

int ProcessData::getProcessCountInGroup(int group) {
    std::lock_guard _{groupListMutex};
    return (int) groupList[group].size();
}

std::set<int> ProcessData::getProcessSetFromGroup(int group) {
    std::lock_guard _{groupListMutex};
    return groupList[group];
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

int ProcessData::getGroupId() const {
    return groupId;
}

void ProcessData::setGroupId(int groupId) {
    ProcessData::groupId = groupId;
}

void ProcessData::setCurrentGroup(std::set<int> group) {
    std::lock_guard _{currentGroupMutex};
    this->currentGroup = group;
}

std::set<int> &ProcessData::getCurrentGroup() {
    std::lock_guard _{currentGroupMutex};
    return currentGroup;
}

void ProcessData::removeProcessFromCurrentGroup(int processId) {
    std::lock_guard _{currentGroupMutex};
    currentGroup.erase(processId);
}

int ProcessData::getRequestClock() const {
    return requestClock;
}

void ProcessData::setRequestClock(int clock) {
    std::lock_guard _{requestClockGroupMutex};
    requestClock = clock;
}
