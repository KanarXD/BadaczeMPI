#pragma once


#include <mutex>
#include <vector>
#include "Settings.h"
#include "../enums/ProcessState.h"

class ProcessData {
private:
    std::mutex clockMutex;
    std::mutex groupListMutex;
    std::mutex processStateMutex;
    int clock = 0;
    int processId;
    int processCount;
    std::vector<std::vector<int>> groupList;
    const Settings settings;
    ProcessState processState = ProcessState::REQUESTING_UNR;
public:
    ProcessData(int processId, int processCount, int groupSize, const Settings& settings);

    [[nodiscard]] int getClock() const;

    [[nodiscard]] int getProcessId() const;

    [[nodiscard]] int getProcessCount() const;

    [[nodiscard]] const Settings &getSettings() const;

    int incrementClock();

    void setClock(int newClock);

    void addProcessToGroup(int group, int process);

    ProcessState getProcessState() const;

    void setProcessState(ProcessState processState);

};



