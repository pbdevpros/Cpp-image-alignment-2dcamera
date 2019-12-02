/// @file ProcessQueue.cpp

#include "ProcessQueue.h"

void ProcessQueue::NotifyCompletion()
{
    std::lock_guard<std::mutex> lck(mutex_);
    isSafeToContinue_ = true;
    cond_.notify_one(); // notify that the process is complete
}

void ProcessQueue::Wait()
{
    // perform queue modification under the lock
    std::unique_lock<std::mutex> lck(mutex_);
    cond_.wait(lck, [this] { return isSafeToContinue_; }); // pass unique lock to condition variable
    isSafeToContinue_ = false;
    return ;
}