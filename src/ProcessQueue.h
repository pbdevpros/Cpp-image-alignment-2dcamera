/// ProcessQueue.h
/// Creator: Pádraig Basquel

#ifndef PROCESSQUEUE
#define PROCESSQUEUE

#include <thread>
#include <queue>
#include <future>
#include <mutex>

/**
 * Class is essentially a wrapper for a condition variable.
 * Management of lock/unlock is managed by the class, abstracting multi-threading features for the developer.
 */

class ProcessQueue
{
public:
    ProcessQueue() { isSafeToContinue_ = true; }
    void Wait();
    void NotifyCompletion();

private:
    bool isSafeToContinue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif