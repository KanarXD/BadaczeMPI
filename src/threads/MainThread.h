#pragma once

#include "CommunicationThread.h"
#include "../models/ProcessData.h"
#include "BaseThread.h"
#include "../enums/ResourceType.h"

class MainThread : public BaseThread {
private:
    CommunicationThread communicationThread;
public:
    explicit MainThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

private:
    void RequestResource(ResourceType resourceType, int responseCount);
    static void Sleep();
};

