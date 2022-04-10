#ifndef __THREAD_SAVE_QUEUE_H
#define __THREAD_SAVE_QUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <memory>
#include <iostream>
#include <thread>

template <class T>
class thread_save_queue final
{
public:
    thread_save_queue() = default;
    ~thread_save_queue() = default;

    void push(const T &val)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(val);
        cond_var_.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while (queue_.empty() && start_) {
            cond_var_.wait(lock);
        }

        T val = queue_.front();
        queue_.pop();
        return val;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return queue_.empty();
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        start_ = false;
        cond_var_.notify_all();
    }
private:
    bool start_ = true;
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable  cond_var_;
};

#endif //__THREAD_SAVE_QUEUE_H