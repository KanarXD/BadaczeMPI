
#include "MainThread.h"
#include "../pch.h"


MainThread::MainThread(std::shared_ptr<ProcessData> &processData) : BaseThread(processData),
                                                                    communicationThread(processData) {
    std::srand(processData->getProcessId());
}

void MainThread::Start() {
    setRunning(true);
    communicationThread.Start();
    int sentMessagesInGroupCounter;
    while (isRunning()) {
        groupListToString();
        switch (getProcessData()->getProcessState()) {
            case REQUESTING_UNR: {
                int unrCount = getProcessData()->getSettings().processCount -
                               getProcessData()->getSettings().UNRCount;
                LOGDEBUG("Requesting UNR, count: ", unrCount);
                requestResource(ResourceType::UNR, unrCount);
                getProcessData()->setRequestClock(INT32_MAX);
                LOG("HAS UNR");
                int groupId = (int) (Functions::getRandomNumber() % getProcessData()->getSettings().GroupCount);
                getProcessData()->setGroupId(groupId);
                getProcessData()->addProcessToGroup(groupId, getProcessData()->getProcessId());
                getProcessData()->setProcessState(ProcessState::REQUESTING_GROUP);
                LOG("Requesting GROUP: ", getProcessData()->getGroupId());
            }
                break;
            case REQUESTING_GROUP: {
                int groupWaitCount =
                        getProcessData()->getSettings().processCount - getProcessData()->getSettings().groupSize;
                LOGDEBUG("Requesting GROUP");

                sentMessagesInGroupCounter = 0;
                getProcessData()->setCurrentGroup(
                        getProcessData()->getProcessSetFromGroup(getProcessData()->getGroupId()));
                requestResource(ResourceType::GROUP, groupWaitCount);
                getProcessData()->setRequestClock(INT32_MAX);
                getProcessData()->setProcessState(ProcessState::IN_GROUP);
                LOG("In GROUP");
            }
                break;
            case IN_GROUP:
                if (sentMessagesInGroupCounter > 0 && Functions::makeDecision(30)) {
                    LOG("Leaving group");
                    getProcessData()->removeProcessFromGroup(getProcessData()->getGroupId(),
                                                             getProcessData()->getProcessId());
                    releaseResource(ResourceType::GROUP);
                    getProcessData()->setRequestClock(INT32_MAX);
                    LOG("Releasing UNR");
                    getProcessData()->setGroupId(-1);
                    releaseResource(ResourceType::UNR);
                    getProcessData()->setRequestClock(INT32_MAX);
                    getProcessData()->setProcessState(ProcessState::SLEEPING);

                    LOG("Left group");
                } else {
                    sendMessageInGroup(sentMessagesInGroupCounter);
                }
                break;
            case SLEEPING:
                if (Functions::makeDecision(30)) {
                    getProcessData()->setProcessState(ProcessState::REQUESTING_UNR);
                    LOG("Requesting UNR");
                }
                break;
        }
        Functions::sleep();
    }
    communicationThread.Stop();
}

void MainThread::requestResource(ResourceType resourceType, int responseCount) {
    if (responseCount > 0) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(responseCount);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::REQUEST,
                    resourceType,
                    getProcessData()->getGroupId()};

    getProcessData()->setRequestClock(getProcessData()->getClock());
    LOG("Sending request messages: ", message, ", waiting for: ", responseCount, " ACK");
    sendToAll(message);

    if (responseCount > 0) {
        std::lock_guard _{getProcessData()->getWaitResourceMutex()};
    }
}

void MainThread::releaseResource(ResourceType resourceType) {
    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType,
                    getProcessData()->getGroupId()};

    if (resourceType == GROUP && getProcessData()->getSettings().processCount > 1) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(getProcessData()->getSettings().processCount - 1);
        getProcessData()->setRequestClock(getProcessData()->getClock());
    }

    LOG("Sending release messages: ", message, " to: ALL");

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


void MainThread::sendMessageInGroup(int &messageCounter) {
    std::set<int> currentGroup = getProcessData()->getCurrentGroup();
    if (currentGroup.size() <= 1) {
        return;
    }
    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::GROUP_TALK,
                    ResourceType::NONE,
                    getProcessData()->getGroupId()};

    groupToString(currentGroup);
    for (int processId: currentGroup) {
        if (processId != getProcessData()->getProcessId()) {
            LOG("Sending group talk messages: ", message, " to: ", processId);
            MPI_Send(&message, sizeof(Message), MPI_BYTE, processId, 0, MPI_COMM_WORLD);
            messageCounter++;
        }
    }
}

void MainThread::groupListToString() const {
    std::stringstream groupListString;
    groupListString << "groups: ";
    for (int i = 0; i < getProcessData()->getSettings().GroupCount; ++i) {
        int groupSize = getProcessData()->getProcessCountInGroup(i);
        if (groupSize > 0) {
            groupListString << "g: " << i;
            groupListString << " = (";
            std::set<int> groupSet = getProcessData()->getProcessSetFromGroup(i);
            for (int process: groupSet) {
                groupListString << process << ", ";
            }
            groupListString << "), ";
        }
    }
    LOGSTATE(groupListString.str().c_str());
}

void MainThread::groupToString(std::set<int> group) const {
    std::stringstream groupListString;
    groupListString << " g: ";
    if (!group.empty()) {
        groupListString << " (";
        for (int process: group) {
            groupListString << process << ", ";
        }
        groupListString << "), ";
    }
    LOGSTATE("Current group: ", getProcessData()->getGroupId(), groupListString.str().c_str());
}

