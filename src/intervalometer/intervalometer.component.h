#ifndef INTERVALOMETER_COMPONENT_H
#define INTERVALOMETER_COMPONENT_H

#include <common/mediator.cpp>
#include <common/event_names.h>

class IntervalometerComponent : public BaseComponent {
public:
    int intervalCounter = 0;
    int workingTimeSec = 0;
    int idleTimeSec = 0;
    long lastIntervalTime = 0;
    long estimatedFinishTime = 0;
    bool workStatus = false;

    void loop(long savedEpochTime);

    void setInterval(int intervals);

    void setWorkingTime(int workTime);

    void setIdleTime(int idleTime);

    void setLastIntervalTime(long time);

    void setEstimatedFinishTime(long lastIntTime, int intervals, int workTime, int idleTime);

    void setWorkStatus(bool flag);
};

#endif
