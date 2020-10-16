#include "my_semaphore.h"
#ifndef _WIN32
semaphore::semaphore()
{
    counter=0;
}

semaphore::semaphore(const char *Name)
{
    List.push_front(SemaphoreID(Name,this));
    counter=0;
}
#endif
