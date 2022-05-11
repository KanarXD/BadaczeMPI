
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
                requestResource(ResourceType::UNR, getProcessData()->getSettings().processCount -
                                                   getProcessData()->getSettings().UNRCount);
                LOGINFO("Changing state to: ", ProcessState::REQUESTING_GROUP);
                getProcessData()->setProcessState(ProcessState::REQUESTING_GROUP);
                break;
            case REQUESTING_GROUP:
                getProcessData()->setProcessState(ProcessState::RELEASING);
                break;
                requestResource(ResourceType::GROUP, getProcessData()->getSettings().processCount -
                                                     getProcessData()->getSettings().UNRCount);
                break;
            case IN_GROUP:
                if (Functions::makeDecision(30)) {

                }
                break;
            case RELEASING:
//                releaseResource(ResourceType::GROUP);
                releaseResource(ResourceType::UNR);
                break;
            case SLEEPING:
                if (getProcessData()->getProcessId() == 0) {
                    getProcessData()->setProcessState(ProcessState::REQUESTING_UNR);
                }
                break;
        }
        Functions::sleep();
    }
    communicationThread.Stop();
}

void MainThread::requestResource(ResourceType resourceType, int responseCount) {
    getProcessData()->getWaitResourceMutex().lock();
    getProcessData()->setAckCount(responseCount);
    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::REQUEST,
                    resourceType};
    LOGINFO("Sending request messages: ", message);
    for (int i = 0; i < getProcessData()->getSettings().processCount; ++i) {
        MPI_Send(&message, sizeof(Message), MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }
    std::lock_guard _{getProcessData()->getWaitResourceMutex()};
}

void MainThread::releaseResource(ResourceType resourceType) {
    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType};
    LOGINFO("Sending release messages: ", message);
    for (int i = 0; i < getProcessData()->getSettings().processCount; ++i) {
        MPI_Send(&message, sizeof(Message), MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }
}


