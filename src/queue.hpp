#ifndef BLOCKS_QUEUE_H
#define BLOCKS_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class TQueue {
public:
    void push(T value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        condition.notify_one();
    };

    void pop(T &value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]{return !queue.empty();});
        value = queue.front();
        queue.pop();
    };

    bool empty()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    };

private:
    std::mutex mutex;
    std::queue<T> queue;
    std::condition_variable condition;
};

#endif
