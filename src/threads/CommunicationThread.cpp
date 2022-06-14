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
        LOGDEBUG("Received message: ", message);

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
            case GROUP_TALK:
                handleGroupTalk(message);
                break;
            case END:
                break;
        }
    }
}

void CommunicationThread::handleRequest(const Message &message) {
    if (message.resourceType == GROUP) {
        LOGDEBUG("added process: ", message.processId, " to group: ", message.groupId);
        getProcessData()->addProcessToGroup(message.groupId, message.processId);
    }
    if (
            getProcessData()->getProcessState() == SLEEPING
            ||
            ((getProcessData()->getProcessState() == REQUESTING_UNR
              ||
              (getProcessData()->getProcessState() == REQUESTING_GROUP && message.resourceType == GROUP))
             &&
             (message.clock < getProcessData()->getRequestClock()
              ||
              (message.clock == getProcessData()->getRequestClock() &&
               message.processId < getProcessData()->getProcessId())))
            ||
            (getProcessData()->getProcessState() == REQUESTING_UNR && message.resourceType == GROUP)
            ||
            ((getProcessData()->getProcessState() == IN_GROUP ||
              getProcessData()->getProcessState() == REQUESTING_GROUP)
             &&
             message.resourceType == GROUP
             &&
             message.groupId != getProcessData()->getGroupId())
            ) {
        sendAck(message);
    }
}

void CommunicationThread::sendAck(const Message &incomingMessage) {
    Message outgoingMessage{getProcessData()->getProcessId(),
                            getProcessData()->getClock(),
                            MessageType::ACK,
                            incomingMessage.resourceType,
                            incomingMessage.groupId};
    LOGDEBUG("Sending ack message: ", outgoingMessage, " to: ", incomingMessage.processId);

    MPI_Send(&outgoingMessage, sizeof(Message), MPI_BYTE, incomingMessage.processId, 0, MPI_COMM_WORLD);
}

void CommunicationThread::handleAck(const Message &message) {
    LOGDEBUG("handleAck message: ", message, "ack left: ", getProcessData()->getAckCount());
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
    LOGDEBUG("releaseMainThread, ack: ", getProcessData()->getAckCount());
    if (getProcessData()->getAckCount() <= 0) {
        return;
    }
    getProcessData()->setAckCount(getProcessData()->getAckCount() - 1);
    if (getProcessData()->getAckCount() == 0) {
        getProcessData()->setRequestClock(INT32_MAX);
        getProcessData()->getWaitResourceMutex().unlock();
    }
    LOGDEBUG("releaseMainThread end, ack: ", getProcessData()->getAckCount());
}

void CommunicationThread::handleRelease(Message incomingMessage) {
    if (incomingMessage.resourceType == GROUP) {
        LOGDEBUG("handleRelease, Sending ACK to id: ", incomingMessage.processId);
        getProcessData()->removeProcessFromCurrentGroup(incomingMessage.processId);
        getProcessData()->removeProcessFromGroup(incomingMessage.groupId, incomingMessage.processId);
        Message outgoingMessage{getProcessData()->getProcessId(),
                                getProcessData()->getClock(),
                                MessageType::ACK,
                                incomingMessage.resourceType,
                                incomingMessage.groupId};
        LOGDEBUG("Sending release ACK: ", outgoingMessage, " to: ", incomingMessage.processId);
        MPI_Send(&outgoingMessage, sizeof(Message), MPI_BYTE, incomingMessage.processId, 0, MPI_COMM_WORLD);
    }
    handleAck(incomingMessage);
}

void CommunicationThread::handleGroupTalk(Message message) {
    LOGDEBUG("Handling group talk: ", message);
    getProcessData()->getCurrentGroup().insert(message.processId);
}






