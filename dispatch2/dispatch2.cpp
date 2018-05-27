#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <boost/any.hpp>

template<typename Func>
class Dispatcher
{
public:
	void register_handler(const std::string& name, const Func& f)
	{
		if (invokers_.find(name) != invokers_.end())
			throw std::invalid_argument("this key has already exist!");

		//boost::any a(f);
		invokers_.emplace(name, f);
	}

	template <typename... Args>
	int call(const std::string& name, Args... args)
	{
		auto it = invokers_.find(name);
		if (it == invokers_.end())
			return -1;

	    //boost::any resolver = invokers_[name];
		//std::function<int (Args...)> function = boost::any_cast<std::function<int (Args...)>>(resolver);
		std::function<int (Args...)> function = invokers_[name];
		return function(args...);
	}

private:
	std::map<std::string, Func> invokers_;
};

struct Testcase1
{
	int test(const std::string& a)
	{
		std::cout <<  "test1" << a << std::endl;
		return 0;
	}
};

struct Testcase2
{
	int test(const std::string& a)
	{
		std::cout <<  "test2" << a << std::endl;
		return 0;
	}
};

typedef std::function<int(std::string)>  Handler;

int main()
{
	Dispatcher<Handler> dispatcher;
	Testcase1 t1;
	Testcase2 t2;
	
	dispatcher.register_handler("Testcase1", std::bind(&Testcase1::test, &t1, std::placeholders::_1));
	dispatcher.register_handler("Testcase2", std::bind(&Testcase2::test, &t2, std::placeholders::_1));

	dispatcher.call<std::string>("Testcase1", "a");
	dispatcher.call<std::string>("Testcase2", "b");
	
	return 0;
}

