#pragma once


#include <atomic>
#include <thread>
#include "../models/ProcessData.h"
#include "BaseThread.h"

class CommunicationThread : public BaseThread {
private:
    std::thread communicationThread;
public:
    explicit CommunicationThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

    void Stop() override;

private:
    void HandleCommunication();

};

