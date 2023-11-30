#include <stdio.h>

#include "net/UsageEnvironment.h"
#include "base/New.h"

UsageEnvironment* UsageEnvironment::createNew(EventScheduler* scheduler)
{
    if(!scheduler)
        return NULL;
    
    return new UsageEnvironment(scheduler);
    // return New<UsageEnvironment>::allocate(scheduler, threadPool);
}

UsageEnvironment::UsageEnvironment(EventScheduler* scheduler) :
    mScheduler(scheduler)
{

}

UsageEnvironment::~UsageEnvironment()
{

}

EventScheduler* UsageEnvironment::scheduler()
{
    return mScheduler;
}
