#pragma once

#include "kapi.h"

namespace Core
{

class Event
{
public:
    Event();
    void Set();
    void SetAll();
    void Wait();
    void Wait(unsigned long timeout);
    void Reset();
    virtual ~Event();

private:
    Event(const Event& other) = delete;
    Event(Event&& other) = delete;
    Event& operator=(const Event& other) = delete;
    Event& operator=(Event&& other) = delete;
    struct kapi_completion Completion;
};

}