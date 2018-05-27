#include <queue>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <memory>
#include <iostream>
#include <thread>

template <class T>
class syncqueue final
{
public:
    syncqueue() = default;
    ~syncqueue() = default;

    void push_front(const T &val)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push(val);
        cond_var_.notify_one();
    }

    T pop_front()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while (queue_.empty()) 
        {
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

private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable  cond_var_;
};
