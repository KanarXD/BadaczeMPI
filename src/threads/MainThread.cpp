
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
            case REQUESTING_GROUP:
                getProcessData()->setGroupId(rand() % getProcessData()->getSettings().GroupCount);
                requestResource(ResourceType::GROUP,
                                getProcessData()->getSettings().processCount - getProcessData()->getSettings().UNRCount,
                                getProcessData()->getGroupId());
                getProcessData()->setProcessState(ProcessState::IN_GROUP);
                LOG("In GROUP");
                break;
            case IN_GROUP:
                if (Functions::makeDecision(30)) {
                    LOG("Leaving group");
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
    getProcessData()->getWaitResourceMutex().lock();
    getProcessData()->setAckCount(responseCount);

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::REQUEST,
                    resourceType,
                    groupId};

    LOG("Sending request messages: ", message);
    for (int i = 0; i < getProcessData()->getSettings().processCount; ++i) {
        MPI_Send(&message, sizeof(Message), MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }
    std::lock_guard _{getProcessData()->getWaitResourceMutex()};
}

void MainThread::releaseResource(ResourceType resourceType) {
    if (resourceType == GROUP) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(getProcessData()->getSettings().processCount);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType};

    LOG("Sending release messages: ", message);

    for (int i = 0; i < getProcessData()->getSettings().processCount; ++i) {
        MPI_Send(&message, sizeof(Message), MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }

    if (resourceType == GROUP) {
        std::lock_guard _{getProcessData()->getWaitResourceMutex()};
    }
}


