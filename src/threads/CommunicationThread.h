#pragma once


#include <atomic>
#include <thread>
#include "../models/ProcessData.h"
#include "BaseThread.h"
#include "../models/Message.h"

class CommunicationThread : public BaseThread {
private:
    std::thread communicationThread;
    std::vector<int> unrWaitingList;
    std::vector<int> groupWaitingList;
public:
    explicit CommunicationThread(std::shared_ptr<ProcessData> &processData);

    void Start() override;

    void Stop() override;

private:
    void HandleCommunication();

    void handleRequest(const Message& message);

    void sendAck(const Message& incomingMessage);

    void addToWaitingList(const Message &message);
};

