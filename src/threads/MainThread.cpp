
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
                sentMessagesInGroupCounter = 0;
                getProcessData()->setCurrentGroup(getProcessData()->getProcessSetFromGroup(groupId));
                requestResource(ResourceType::GROUP,
                                groupWaitCount);
                getProcessData()->setProcessState(ProcessState::IN_GROUP);
                LOGDEBUG("In GROUP");
            }
                break;
            case IN_GROUP:
                if (sentMessagesInGroupCounter > 0 && Functions::makeDecision(30)) {
                    LOGDEBUG("Leaving group");
                    getProcessData()->removeProcessFromGroup(getProcessData()->getGroupId(),
                                                             getProcessData()->getProcessId());
                    releaseResource(ResourceType::GROUP);
                    LOGDEBUG("Releasing UNR");
                    getProcessData()->setGroupId(-1);
                    releaseResource(ResourceType::UNR);
                    getProcessData()->setProcessState(ProcessState::SLEEPING);
                } else {
                    sendMessageInGroup(sentMessagesInGroupCounter);
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

    LOG("Sending request messages: ", message, ", waiting for: ", responseCount, " ACK");
    sendToAll(message);

    if (responseCount > 0) {
        std::lock_guard _{getProcessData()->getWaitResourceMutex()};
    }
}

void MainThread::releaseResource(ResourceType resourceType) {
    if (resourceType == GROUP && getProcessData()->getSettings().processCount > 1) {
        getProcessData()->getWaitResourceMutex().lock();
        getProcessData()->setAckCount(getProcessData()->getSettings().processCount - 1);
    }

    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::RELEASE,
                    resourceType,
                    getProcessData()->getGroupId()};

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


void MainThread::sendMessageInGroup(int &messageCounter) {
    Message message{getProcessData()->getProcessId(),
                    getProcessData()->incrementClock(),
                    MessageType::GROUP_TALK,
                    ResourceType::NONE,
                    getProcessData()->getGroupId()};

    std::set<int> currentGroup = getProcessData()->getCurrentGroup();
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
        groupListString << "g: " << i;
        if (groupSize > 0) {
            groupListString << " (";
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

