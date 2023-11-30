#ifndef _USAGEENVIRONMENT_H_
#define _USAGEENVIRONMENT_H_
#include "net/EventScheduler.h"

class UsageEnvironment
{
public:
    static UsageEnvironment* createNew(EventScheduler* scheduler);
    
    UsageEnvironment(EventScheduler* scheduler);
    ~UsageEnvironment();

    EventScheduler* scheduler();

private:
    EventScheduler* mScheduler;
};

#endif //_USAGEENVIRONMENT_H_