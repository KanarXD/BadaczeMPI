#include "CommunicationThread.h"
#include "../pch.h"

CommunicationThread::CommunicationThread(std::shared_ptr<ProcessData> &processData) : BaseThread(processData) {

}

void CommunicationThread::Start() {
    setRunning(true);
    communicationThread = std::thread(&CommunicationThread::HandleCommunication, this);
}

void CommunicationThread::Stop() {
    setRunning(false);
    communicationThread.join();
}

void CommunicationThread::HandleCommunication() {
    while (isRunning()) {

        Message message{};
        MPI_Status status;
        MPI_Recv(&message, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        getProcessData()->setClock(message.clock);


        switch (message.messageType) {
            case REQUEST:
                handleRequest(message);
                break;
            case ACK:
                handleAck(message);
                break;

        }
        LOG("Received message: ", message);

    }
}

void CommunicationThread::handleRequest(const Message &message) {
    if (getProcessData()->getProcessState() == SLEEPING || getProcessData()->getProcessState() == RELEASING) {
        sendAck(message);
    } else {
        addToWaitingList(message);
    }
}

void CommunicationThread::addToWaitingList(const Message &message) {
    switch (message.resourceType) {
        case UNR:
            unrWaitingList.push_back(message.processId);
            break;
        case GROUP:
            groupWaitingList.push_back(message.processId);
            break;
    }
}

void CommunicationThread::sendAck(const Message &incomingMessage) {
    Message outgoingMessage{getProcessData()->getProcessId(),
                            getProcessData()->incrementClock(),
                            MessageType::ACK,
                            incomingMessage.resourceType};
    LOG("Sending ACK to: ", incomingMessage.processId);
    MPI_Send(&outgoingMessage, sizeof(Message), MPI_BYTE, incomingMessage.processId, 0, MPI_COMM_WORLD);
}

void CommunicationThread::handleAck(const Message &message) {
    if ((message.resourceType == ResourceType::UNR &&
         getProcessData()->getProcessState() == ProcessState::REQUESTING_UNR)
        ||
        (message.resourceType == ResourceType::GROUP &&
         getProcessData()->getProcessState() == ProcessState::REQUESTING_GROUP)) {
        getProcessData()->setAckCount(getProcessData()->getAckCount() - 1);
        if (getProcessData()->getAckCount() <= 0) {
            getProcessData()->getWaitResourceMutex().unlock();
        }
    }
}






