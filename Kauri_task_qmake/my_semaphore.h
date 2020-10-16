#ifndef MY_SEMAPHORE_H
#define MY_SEMAPHORE_H
#include <forward_list>
#ifndef _WIN32
class semaphore
{
    struct SemaphoreID
    {
        int ID;
        const char *name;
        semaphore *ptr;
        SemaphoreID(const char *Name, semaphore *Ptr)
        {name=Name; ptr=Ptr;}
    };
    int counter;
public:
    static std::forward_list<SemaphoreID> List;
    semaphore();
    semaphore(const char *Name);
    ~semaphore();
    void Passing() {counter++; while(counter);}
    void Release() {counter--;}
};
#endif

#endif // SEMAPHORE_H
