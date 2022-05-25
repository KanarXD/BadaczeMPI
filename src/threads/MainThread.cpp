
#include "MainThread.h"
#include "../pch.h"


MainThread::MainThread(std::shared_ptr<ProcessData> &processData) : BaseThread(processData),
                                                                    communicationThread(processData) {
    std::srand(processData->getProcessId());
}

void MainThread::Start() {
    setRunning(true);
    communicationThread.Start();
    while (isRunning()) {
        switch (getProcessData()->getProcessState()) {
            case REQUESTING_UNR:
                LOG("Requesting UNR");
                requestResource(ResourceType::UNR, getProcessData()->getSettings().processCount -
                                                   getProcessData()->getSettings().UNRCount, 0);
                getProcessData()->setProcessState(ProcessState::REQUESTING_GROUP);
                LOG("Requesting GROUP");
                break;
            case REQUESTING_GROUP: {
                int groupId = rand() % getProcessData()->getSettings().GroupCount;
                int groupFreeSlots =
                        getProcessData()->getSettings().GroupCount - getProcessData()->getProcessCountInGroup(groupId);

                getProcessData()->setGroupId(groupId);
//                getProcessData()->addProcessToGroup(groupId, getProcessData()->getProcessId());

                requestResource(ResourceType::GROUP,
                                getProcessData()->getSettings().processCount - groupFreeSlots,
                                getProcessData()->getGroupId());

                getProcessData()->setProcessState(ProcessState::IN_GROUP);
                LOG("In GROUP");
            }
                break;
            case IN_GROUP:
                if (Functions::makeDecision(30)) {
                    LOG("Leaving group");
//                    getProcessData()->removeProcessFromGroup(getProcessData()->getGroupId(),
//                                                             getProcessData()->getProcessId());
                    releaseResource(ResourceType::GROUP);
                    LOG("Releasing UNR");
                    releaseResource(ResourceType::UNR);
                    getProcessData()->setProcessState(ProcessState::SLEEPING);
                }
                break;
            case SLEEPING:
                if (Functions::makeDecision(30)) {
                    getProcessData()->setProcessState(ProcessState::REQUESTING_UNR);
                }
                break;
        }
        Functions::sleep();
    }
    communicationThread.Stop();
}

void MainThread::requestResource(ResourceType resourceType, int responseCount, int groupId) {
    if (responseCount <= 0) {
        return;
    }
    getProcessData()->getWaitResourceMutex().lock();
    getProcessData()->setAckCount(responseCount);

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::REQUEST,
                    resourceType,
                    groupId};

    LOG("Sending request messages: ", message, ", waiting for: ", responseCount, " ACK");
    sendToAll(message);

    std::lock_guard _{getProcessData()->getWaitResourceMutex()};
}

void MainThread::releaseResource(ResourceType resourceType) {
    if (resourceType == GROUP && getProcessData()->getSettings().processCount > 1) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(getProcessData()->getSettings().processCount - 1);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType};

    LOG("Sending release messages: ", message);

    sendToAll(message);

    if (resourceType == GROUP && getProcessData()->getSettings().processCount > 1) {
        std::lock_guard _{getProcessData()->getWaitResourceMutex()};
    }
}

void MainThread::sendToAll(const Message &message) const {
    for (int i = 0; i < getProcessData()->getSettings().processCount; ++i) {
        if (i != getProcessData()->getProcessId()) {
            MPI_Send(&message, sizeof(Message), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
    }
}




