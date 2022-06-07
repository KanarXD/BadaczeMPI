#pragma once

#include <ostream>
#include "../enums/MessageType.h"
#include "../enums/ResourceType.h"

struct Message {
    int processId;
    int clock;
    MessageType messageType;
    ResourceType resourceType;
    int groupId;

    friend std::ostream &operator<<(std::ostream &os, const Message &message) {
        os << "(processId: " << message.processId << " clock: " << message.clock << " messageType: "
           << getMessageType(message.messageType) << " resourceType: " << getResourceName(message.resourceType)
           << " groupId: " << message.groupId << ')';
        return os;
    }

    static std::string getResourceName(ResourceType type) {
        switch (type) {
            case NONE:
                return "NONE";
            case UNR:
                return "UNR";
            case GROUP:
                return "GROUP";
        }
    }

    static std::string getMessageType(MessageType type) {
        switch (type) {
            case END:
                return "END";
            case REQUEST:
                return "REQUEST";
            case RELEASE:
                return "RELEASE";
            case ACK:
                return "ACK";
            case GROUP_TALK:
                return "GROUP_TALK";
        }
    }
};