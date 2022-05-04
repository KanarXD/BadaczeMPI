#pragma once

#include "CommunicationThread.h"
#include "../models/ProcessData.h"
#include "BaseThread.h"

class MainThread : public BaseThread {
private:
    CommunicationThread communicationThread;
public:
    explicit MainThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

private:
    static void sleep();
};

