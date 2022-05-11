#pragma once

using State = unsigned short;

enum ProcessState : State {
    RELEASING = 4,
    IN_GROUP = 3,
    REQUESTING_GROUP = 2,
    REQUESTING_UNR = 1,
    SLEEPING = 0,

};