#pragma once


enum ProcessState : unsigned short {
    IN_GROUP = 3,
    REQUESTING_GROUP = 2,
    REQUESTING_UNR = 1,
    SLEEPING = 0,

};
