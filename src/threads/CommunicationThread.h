#pragma once

#include <thread>
#include "BaseThread.h"
#include "../models/ProcessData.h"
#include "../models/Message.h"


class CommunicationThread : public BaseThread {
private:
    std::thread communicationThread;
public:
    explicit CommunicationThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

    void Stop() override;

private:
    void HandleCommunication();

    void handleRequest(const Message &message);

    void sendAck(const Message &incomingMessage);

    void addToWaitingList(const Message &message);

    void handleAck(const Message &message);

    void handleRelease(Message incomingMessage);

    void releaseMainThread() const;
};

