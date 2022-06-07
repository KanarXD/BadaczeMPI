#pragma once


enum MessageType : unsigned short {
    GROUP_TALK = 4,
    ACK = 3,
    RELEASE = 2,
    REQUEST = 1,
    END = 0
};


