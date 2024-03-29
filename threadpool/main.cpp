#include <sstream>
#include <iostream>
#include "threadpool.hpp"

struct Test
{
    int task(const std::string& x)
    {
        std::cout << "test: " << x << std::endl;
        return 0;
    }
};

int main()
{
    Test test;
    auto thrdpool = std::make_shared<Threadpool>(5);
    for (int i = 0; i < 10; i++) {
        thrdpool->addtask(std::bind(&Test::task, &test, std::to_string(i)));
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}