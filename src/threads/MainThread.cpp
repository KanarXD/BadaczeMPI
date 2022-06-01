
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
        groupListToString();
        switch (getProcessData()->getProcessState()) {
            case REQUESTING_UNR: {
                int unrCount = getProcessData()->getSettings().processCount -
                               getProcessData()->getSettings().UNRCount;
                LOGDEBUG("Requesting UNR, count: ", unrCount);
                requestResource(ResourceType::UNR, unrCount, 0);
                getProcessData()->setProcessState(ProcessState::REQUESTING_GROUP);
                LOGDEBUG("Requesting GROUP");
            }
                break;
            case REQUESTING_GROUP: {
                int groupId = (int) (Functions::getRandomNumber() % getProcessData()->getSettings().GroupCount);
                getProcessData()->addProcessToGroup(groupId, getProcessData()->getProcessId());
                int groupWaitCount =
                        getProcessData()->getSettings().processCount - getProcessData()->getSettings().groupSize;
                LOGDEBUG("Requesting GROUP");
                getProcessData()->setGroupId(groupId);
                requestResource(ResourceType::GROUP,
                                groupWaitCount,
                                getProcessData()->getGroupId());

                getProcessData()->setProcessState(ProcessState::IN_GROUP);
                LOGDEBUG("In GROUP");
            }
                break;
            case IN_GROUP:
                if (Functions::makeDecision(30)) {
                    LOGDEBUG("Leaving group");
                    getProcessData()->removeProcessFromGroup(getProcessData()->getGroupId(),
                                                             getProcessData()->getProcessId());
                    releaseResource(ResourceType::GROUP, getProcessData()->getGroupId());
                    LOGDEBUG("Releasing UNR");
                    releaseResource(ResourceType::UNR, -1);
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

void MainThread::groupListToString() const {
    std::stringstream groupListString;
    groupListString << "groups: ";
    for (int i = 0; i < getProcessData()->getSettings().GroupCount; ++i) {
        int groupSize = getProcessData()->getProcessCountInGroup(i);
        groupListString << "g: " << i << "s: ";
        if (groupSize > 0) {
            groupListString << groupSize << " (";
            std::set<int> groupSet = getProcessData()->getProcessSetFromGroup(i);
            for (int process: groupSet) {
                groupListString << process << ", ";
            }
            groupListString << "), ";
        }
    }
    LOGSTATE(groupListString.str().c_str());
}

void MainThread::requestResource(ResourceType resourceType, int responseCount, int groupId) {
    if (responseCount > 0) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(responseCount);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::REQUEST,
                    resourceType,
                    groupId};

    LOG("Sending request messages: ", message, ", waiting for: ", responseCount, " ACK");
    sendToAll(message);

    if (responseCount > 0) {
        std::lock_guard _{getProcessData()->getWaitResourceMutex()};
    }
}

void MainThread::releaseResource(ResourceType resourceType, int groupId) {
    if (resourceType == GROUP && getProcessData()->getSettings().processCount > 1) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(getProcessData()->getSettings().processCount - 1);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType,
                    groupId};

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




