#ifndef EVENT_NAMES_H
#define EVENT_NAMES_H

#include <Arduino.h>

typedef const char eventNameType[5];

struct EventNames {
    eventNameType clientNewEventName = "clie";

    eventNameType newUsersEventName = "nusr";

    eventNameType connectedUsersEventName = "user";
    eventNameType timeEventName = "time";

    eventNameType gateEventName = "gate";
    eventNameType goalGatePositionEventName = "gapo";

    eventNameType lightStatusEventName = "list";

    eventNameType shutterEventName = "shut";

    eventNameType intervalsEventName = "inte";
    eventNameType workingTimeEventName = "woti";
    eventNameType idleTimeEventName = "idti";
    eventNameType lastIntervalTimeEventName = "lati";
    eventNameType estimatedFinishTimeEventName = "esti";
    eventNameType workStatusEventName = "wost";

    eventNameType isOpenGateAtWorkEventName = "isga";
    eventNameType isLightOnAtWorkEventName = "isli";
    eventNameType isPressButtonAtWorkEventName = "isbu";
};

constexpr EventNames eventNames;

#endif
