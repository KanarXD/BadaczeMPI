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

        LOG("Received message: ", message);
        switch (message.messageType) {
            case REQUEST:
                handleRequest(message);
                break;
            case ACK:
                handleAck(message);
                break;
            case RELEASE:
                handleRelease(message);
                break;
            case END:
                break;
        }
        getProcessData()->setClock(message.clock);

    }
}

void CommunicationThread::handleRequest(const Message &message) {
    if (message.resourceType == GROUP) {
        LOG("added process: ", message.processId, " to group: ", message.groupId);
        getProcessData()->addProcessToGroup(message.groupId, message.processId);
    }
    if (
            getProcessData()->getProcessState() == SLEEPING ||
            message.clock < getProcessData()->getClock() ||
            (message.clock == getProcessData()->getClock() && message.processId < getProcessData()->getProcessId())
            ) {

        sendAck(message);
    } else if (
            (getProcessData()->getProcessState() == REQUESTING_UNR && message.resourceType == GROUP)
            ||
            (getProcessData()->getProcessState() == IN_GROUP && message.resourceType == GROUP &&
             message.groupId != getProcessData()->getGroupId())
            ) {
        sendAck(message);
    }
}

void CommunicationThread::sendAck(const Message &incomingMessage) {
    LOG("sendAck, Sending ACK to: ", incomingMessage.processId);
    Message outgoingMessage{getProcessData()->getProcessId(),
                            getProcessData()->incrementClock(),
                            MessageType::ACK,
                            incomingMessage.resourceType};

    MPI_Send(&outgoingMessage, sizeof(Message), MPI_BYTE, incomingMessage.processId, 0, MPI_COMM_WORLD);
}

void CommunicationThread::handleAck(const Message &message) {
    LOG("handleAck, ack: ", getProcessData()->getAckCount());
    if (
            (message.resourceType == ResourceType::UNR &&
             getProcessData()->getProcessState() == ProcessState::REQUESTING_UNR)
            ||
            (message.resourceType == ResourceType::GROUP && (
                    getProcessData()->getProcessState() == ProcessState::REQUESTING_GROUP ||
                    getProcessData()->getProcessState() == ProcessState::IN_GROUP))
            ) {
        releaseMainThread();
    }
}

void CommunicationThread::releaseMainThread() const {
    LOG("releaseMainThread, ack: ", getProcessData()->getAckCount());
    if (getProcessData()->getAckCount() <= 0) {
        return;
    }
    getProcessData()->setAckCount(getProcessData()->getAckCount() - 1);
    if (getProcessData()->getAckCount() == 0) {
        getProcessData()->getWaitResourceMutex().unlock();
    }
    LOG("releaseMainThread end, ack: ", getProcessData()->getAckCount());
}

void CommunicationThread::handleRelease(Message incomingMessage) {
    if (incomingMessage.resourceType == GROUP) {
        LOG("handleRelease, Sending ACK to id: ", incomingMessage.processId);
        LOG("removing process: ", incomingMessage.processId, " from group: ", incomingMessage.groupId);
        getProcessData()->removeProcessFromGroup(incomingMessage.groupId, incomingMessage.processId);
        Message outgoingMessage{getProcessData()->getProcessId(),
                                getProcessData()->incrementClock(),
                                MessageType::ACK,
                                incomingMessage.resourceType};
        MPI_Send(&outgoingMessage, sizeof(Message), MPI_BYTE, incomingMessage.processId, 0, MPI_COMM_WORLD);
    } else if (incomingMessage.resourceType == UNR) {
        handleAck(incomingMessage);
    }
}






