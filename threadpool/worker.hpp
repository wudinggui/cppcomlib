#pragma once
#include <thread>
#include <memory>
#include "syncqueue.hpp"

class Worker;
typedef std::shared_ptr<Worker> Worker_ptr;
typedef std::function<int (const std::string&)> Task;
typedef syncqueue<Task> Taskqueue;
class Worker final
{
public:
	Worker(std::shared_ptr<Taskqueue> &queue, int id)
	{
		start_ = true;
		queue_ = queue;
		workerid_ = id;
		thread_ = std::thread(std::bind(&Worker::run, this));
	}
	
	~Worker()
	{
	}

	void join()
	{
		start_ = false;
		thread_.join();
	}
private:
    void run()
	{
		while (start_)
		{
			if (queue_->empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			
			Task task = queue_->pop_front();
			if (task)
			{
				task("");
			}
		}
	}
	
    bool start_;
	int workerid_;
	std::thread thread_;
	std::shared_ptr<Taskqueue> queue_;
};