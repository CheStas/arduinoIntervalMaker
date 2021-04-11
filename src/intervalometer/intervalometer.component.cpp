#include <intervalometer/intervalometer.component.h>
#include <common/event_names.h>

void IntervalometerComponent::setInterval(int intervals) {
    intervalCounter = intervals;

    this->mediator_->Notify(this, eventNames.intervalsEventName);
}

void IntervalometerComponent::setWorkingTime(int workTime) {
    workingTimeSec = workTime;
    this->mediator_->Notify(this, eventNames.workingTimeEventName);
}

void IntervalometerComponent::setIdleTime(int idleTime) {
    idleTimeSec = idleTime;
    this->mediator_->Notify(this, eventNames.idleTimeEventName);
}

void IntervalometerComponent::setLastIntervalTime(long time) {
    lastIntervalTime = time;
    this->mediator_->Notify(this, eventNames.lastIntervalTimeEventName);
}

void IntervalometerComponent::setWorkStatus(bool flag) {
    workStatus = flag;
    this->mediator_->Notify(this, eventNames.workStatusEventName);
}

void IntervalometerComponent::setEstimatedFinishTime(long lastIntTime, int intervals, int workTime, int idleTime) {
    long savedEstTime = estimatedFinishTime;
    estimatedFinishTime = lastIntTime + (intervals * (workTime + idleTime));
    if (savedEstTime != estimatedFinishTime) {
        this->mediator_->Notify(this, eventNames.estimatedFinishTimeEventName);
    }
}

void IntervalometerComponent::loop(long savedEpochTime) {
    if (intervalCounter && idleTimeSec && workingTimeSec) {
        const int diffFromLastInterval = savedEpochTime - lastIntervalTime;
        if (!workStatus && diffFromLastInterval > idleTimeSec) {

            setWorkStatus(true);
            setLastIntervalTime(savedEpochTime);

            setEstimatedFinishTime(savedEpochTime, intervalCounter, workingTimeSec, idleTimeSec);

            // TODO implement work
            // doWork();
        } else if (workStatus && diffFromLastInterval > workingTimeSec) {

            setWorkStatus(false);

            // TODO implement work
            // endWork();
            setLastIntervalTime(savedEpochTime);
            setInterval(--intervalCounter);
        }

    }
}
