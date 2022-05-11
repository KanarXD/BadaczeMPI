#pragma once

#include "CommunicationThread.h"
#include "BaseThread.h"

class MainThread : public BaseThread {
private:
    CommunicationThread communicationThread;
public:
    explicit MainThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

private:
    void requestResource(ResourceType resourceType, int responseCount);

    void releaseResource(ResourceType type);
};

