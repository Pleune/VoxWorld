#ifndef BLOCKS_RWLOCK_H
#define BLOCKS_RWLOCK_H

#include <atomic>
#include <mutex>

class RWLock {
public:
    enum LockType {READ, WRITE};

    RWLock();
    void lock(LockType);
    void unlock();

private:
    LockType lock_type;
    std::mutex lock_m;
    std::atomic<int> lock_num;
};

#endif
