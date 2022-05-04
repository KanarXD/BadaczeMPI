#pragma once


#include <mutex>
#include <vector>
#include "Settings.h"

class ProcessData {
private:
    std::mutex clockMutex;
    std::mutex groupListMutex;
    int clock = 0;
    int processId;
    int processCount;
    std::vector<std::vector<int>> groupList;
    const Settings settings;
public:
    ProcessData(int processId, int processCount, int groupSize, const Settings& settings);

    [[nodiscard]] int getClock() const;

    [[nodiscard]] int getProcessId() const;

    [[nodiscard]] int getProcessCount() const;

    [[nodiscard]] const Settings &getSettings() const;

    void increaseClock();

    void setClock(int newClock);

    void addProcessToGroup(int group, int process);

};



