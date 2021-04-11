#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <common/event_names.h>

class BaseComponent;

class Mediator {
public:
    virtual void Notify(BaseComponent *sender, eventNameType event);
    virtual void Notify(BaseComponent *sender, eventNameType event, char* name, int value);
    virtual void Notify(BaseComponent *sender, eventNameType event, uint8_t num);
};


class BaseComponent {
protected:
    Mediator *mediator_;

public:
    BaseComponent(Mediator *mediator = nullptr) : mediator_(mediator) {
    }

    void set_mediator(Mediator *mediator) {
        this->mediator_ = mediator;
    }
};

#endif
