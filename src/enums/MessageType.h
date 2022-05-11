#pragma once

using RequestCode = unsigned short;

enum MessageType : RequestCode {
    ACK = 3,
    RELEASE = 2,
    REQUEST = 1,
    END = 0
};


