#pragma once

#include <mutex>
#include <atomic>
#include <vector>
#include "Settings.h"
#include "../enums/ProcessState.h"

class ProcessData {
private:
    std::mutex clockMutex;
    std::mutex groupListMutex;
    std::mutex waitResourceMutex;
    int clock = 0;
    int processId;
    std::vector<std::vector<int>> groupList;
    const Settings settings;
    ProcessState processState = ProcessState::SLEEPING;
    std::atomic<int> ackCount{0};
public:
    ProcessData(int processId, int groupSize, const Settings &settings);

    [[nodiscard]] int getClock() const;

    [[nodiscard]] int getProcessId() const;

    [[nodiscard]] const Settings &getSettings() const;

    int incrementClock();

    void setClock(int newClock);

    void addProcessToGroup(int group, int process);

    [[nodiscard]] ProcessState getProcessState() const;

    void setProcessState(ProcessState state);

    std::mutex &getWaitResourceMutex();

    [[nodiscard]] int getAckCount() const;

    void setAckCount(int count);

};



