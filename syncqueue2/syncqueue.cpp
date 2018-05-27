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

typedef  std::function<int (int)> Task;
typedef  syncqueue<Task>  Syncqueue;

struct Test
{
    int test(int x)
    {
        std::cout <<  "test " << x << std::endl;
        return 0;
    }
};

struct Producer
{
    Producer(std::shared_ptr<Syncqueue>& ptr)
    {
        queue_ = ptr;
    }
    
    void produce()
    {
        for (int i = 0; i < 10; i++)
        {
            Test t;
            Task task = std::bind(&Test::test, &t, i);
            queue_->push_front(task);
            std::cout <<  "produce task" << i << std::endl;
            usleep(1000);
        }
    }
    
private:
    std::shared_ptr<Syncqueue> queue_;
};

struct Consumer
{
    Consumer(std::shared_ptr<Syncqueue>& ptr)
    {
        queue_ = ptr;
    }
    
    void consume()
    {
        while (!queue_->empty())
        {
            Task task = queue_->pop_front();
            std::cout <<  "consume task " << std::endl;
            if (task)
            {
                task(0);
            }
            usleep(1000);
        }
    }
    
private:
    std::shared_ptr<Syncqueue> queue_;
};

int main()
{
    std::shared_ptr<Syncqueue>  queue_ptr(new Syncqueue());
    
    Producer p(queue_ptr);
    Consumer c(queue_ptr);
    
    std::thread p_thrd(&Producer::produce, &p);
    usleep(1000);
    std::thread c_thrd(&Consumer::consume, &c);
    
    p_thrd.join();
    c_thrd.join();
    
    return 0;
}