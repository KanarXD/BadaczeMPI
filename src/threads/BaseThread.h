#pragma once

#include "../models/ProcessData.h"
#include <atomic>
#include <thread>

class BaseThread {
private:
    std::atomic<bool> running{false};
    std::shared_ptr<ProcessData> processData;
public:
    explicit BaseThread(std::shared_ptr<ProcessData> &processData);

    virtual void Start() = 0;

    virtual void Stop() {};

    [[nodiscard]] bool isRunning() const;

    void setRunning(bool running);

    [[nodiscard]] const std::shared_ptr<ProcessData> &getProcessData() const;

};
