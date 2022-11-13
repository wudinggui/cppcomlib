#ifndef __THREAD_SAFE_mDataQueueH
#define __THREAD_SAFE_mDataQueueH
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <thread>

template <class T>
class threadsafe_queue final
{
private:
    std::queue<T> mDataQueue;
    mutable std::mutex mMut;
    std::condition_variable  mDataCV;
public:
    threadsafe_queue() = default;
    ~threadsafe_queue() = default;
    threadsafe_queue(threadsafe_queue const& other) 
    {
        std::lock_guard<std::mutex> lk(other.mMut);
        mDataQueue = other.mDataQueue;
    }

    void push(const T &val)
    {
        std::lock_guard<std::mutex> lk(mMut);
        mDataQueue.push(val);
        mDataCV.notify_one();
    }

    void wait_and_pop(T &val)
    {
        std::unique_lock<std::mutex> lk(mMut);
        mDataCV.wait(lk, [this] { return !mDataQueue.empty(); });

        val = mDataQueue.front();
        mDataQueue.pop();
    }

    std::shared_ptr<T>  wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mMut);
        mDataCV.wait(lk, [this] { return !mDataQueue.empty(); });
        std::shared_ptr<T> val(std::make_shared<T>(mDataQueue.front()));
        mDataQueue.pop();
        return val;
    }

    bool try_pop(T& val)
    {
        std::lock_guard<std::mutex> lk(mMut);
        if (mDataQueue.empty()) {
            return false;
        }

        val = mDataQueue.front();
        mDataQueue.pop();
        return true;
    }

    std::shared_ptr<T>  try_pop()
    {
        std::lock_guard<std::mutex> lk(mMut);
        if (mDataQueue.empty()) {
            return nullptr;
        }

        std::shared_ptr<T> val(std::make_shared<T>(mDataQueue.front()));
        mDataQueue.pop();
        return val;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lk(mMut);
        return mDataQueue.empty();
    }

    void stop_wait_and_pop()
    {
        mDataQueue.push(nullptr);
        mDataCV.notify_one();
    }
};

#endif