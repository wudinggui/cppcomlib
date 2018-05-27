#include <vector>
#include <memory>
#include "worker.hpp"


class Threadpool
{
public:
	Threadpool(int thread_num)
	{
		queue_ = std::shared_ptr<Taskqueue>(new Taskqueue());
		for (int i = 0; i < thread_num; i++)
		{
			auto worker = std::make_shared<Worker>(queue_, i);
			workers_.push_back(worker);
		}
	}
	
	~Threadpool()
	{
		for (auto& worker: workers_)
		{
			worker->join();
		}
		
		workers_.clear();
	}

	void addtask(const Task& task)
	{
		queue_->push_front(task);
	}
private:
    std::vector<std::shared_ptr<Worker>> workers_;
	std::shared_ptr<Taskqueue> queue_;
};
