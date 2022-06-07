#pragma once

#include <mutex>
#include <atomic>
#include <vector>
#include <set>
#include "Settings.h"
#include "../enums/ProcessState.h"

class ProcessData {
private:
    std::mutex clockMutex;
    std::mutex groupListMutex;
    std::mutex waitResourceMutex;
    std::mutex currentGroupMutex;
    int clock = 0;
    int processId;
    std::vector<std::set<int>> groupList;
    const Settings settings;
    ProcessState processState = ProcessState::SLEEPING;
    std::atomic<int> ackCount{0};
    int groupId = -1;
    std::set<int> currentGroup;
public:
    ProcessData(int processId, const Settings &settings);

    [[nodiscard]] int getClock() const;

    [[nodiscard]] int getProcessId() const;

    [[nodiscard]] const Settings &getSettings() const;

    int incrementClock();

    void setClock(int newClock);

    void addProcessToGroup(int group, int process);

    void removeProcessFromGroup(int group, int process);

    std::set<int> getProcessSetFromGroup(int group);

    [[nodiscard]] int getProcessCountInGroup(int group);

    [[nodiscard]] ProcessState getProcessState() const;

    void setProcessState(ProcessState state);

    std::mutex &getWaitResourceMutex();

    [[nodiscard]] int getAckCount() const;

    void setAckCount(int count);

    [[nodiscard]] int getGroupId() const;

    void setGroupId(int groupId);

    void setCurrentGroup(std::set<int> group);

    std::set<int> &getCurrentGroup();

    void removeProcessFromCurrentGroup(int processId);

};



