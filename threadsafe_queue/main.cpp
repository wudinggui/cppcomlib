#include <unistd.h>
#include <memory>
#include <iostream>
#include <memory>
#include "threadsafe_queue.hpp"

typedef  std::function<int ()> Task;
typedef  threadsafe_queue<Task>  taskQueue;

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
    Producer(std::shared_ptr<taskQueue>& ptr)
    {
        mTaskQueue = ptr;
    }
    
    void produce()
    {
        for (int i = 0; i < 10; i++)
        {
            Test t;
            Task task = std::bind(&Test::test, &t, i);
            mTaskQueue->push(task);
            std::cout <<  "produce task" << i << std::endl;
            usleep(1000);
        }
    }
    
private:
    std::shared_ptr<taskQueue> mTaskQueue;
};

struct Consumer
{
    Consumer(std::shared_ptr<taskQueue>& ptr)
    {
        mTaskQueue = ptr;
    }
    
    void consume()
    {
        while (!mTaskQueue->empty())
        {
            Task task = nullptr;
            mTaskQueue->wait_and_pop(task);
            std::cout <<  "consume task " << std::endl;
            if (task) {
                task();
            }
            usleep(1000);
        }
    }
    
private:
    std::shared_ptr<taskQueue> mTaskQueue;
};

int main()
{
    std::shared_ptr<taskQueue>  mTaskQueueptr(new taskQueue());
    
    Producer p(mTaskQueueptr);
    Consumer c(mTaskQueueptr);
    
    std::thread p_thrd(&Producer::produce, &p);
    usleep(1000);
    std::thread c_thrd(&Consumer::consume, &c);
    
    p_thrd.join();
    c_thrd.join();
    
    return 0;
}