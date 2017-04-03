#include "rwlock.hpp"

RWLock::RWLock()
    :lock_num(0)
{
}

void RWLock::lock(LockType type)
{
    if(type == WRITE)
    {
        lock_m.lock();
        lock_type = WRITE;
    } else {
        if(lock_type == WRITE)
        {
            lock_m.lock();
            lock_type = READ;
            lock_num = 1;
        } else {
            if(lock_num == 0)
                lock_m.lock();
            lock_num++;
        }
    }
}

void RWLock::unlock()
{
    if(lock_type == WRITE)
    {
        lock_m.unlock();
    } else {
        lock_num--;
        if(lock_num == 0)
            lock_m.unlock();
    }
}
