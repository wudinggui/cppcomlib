#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <boost/any.hpp>

class Dispatcher
{
public:
    template<typename Func>
    void register_handler(const std::string& name, const Func& f)
    {
        if (invokers_.find(name) != invokers_.end())
            throw std::invalid_argument("this key has already exist!");

        boost::any a(f);
        invokers_.emplace(name, a);
    }

    template <typename... Args>
    int call(const std::string& name, Args... args)
    {
        auto it = invokers_.find(name);
        if (it == invokers_.end())
            return -1;

        boost::any resolver = invokers_[name];
        std::function<int (Args...)> function = boost::any_cast<std::function<int (Args...)>>(resolver);
        return function(args...);
    }

private:
    std::map<std::string, boost::any> invokers_;
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
    int test(const std::string& a, const std::string& b)
    {
        std::cout <<  "test2" << a << b << std::endl;
        return 0;
    }
};

typedef std::function<int(std::string)>  Handler1;
typedef std::function<int(std::string, std::string)>  Handler2;

int main()
{
    Dispatcher dispatcher;
    Testcase1 t1;
    Testcase2 t2;
    
    dispatcher.register_handler<Handler1>("Testcase1", std::bind(&Testcase1::test, &t1, std::placeholders::_1));
    dispatcher.register_handler<Handler2>("Testcase2", std::bind(&Testcase2::test, &t2, std::placeholders::_1, std::placeholders::_2));

    dispatcher.call<std::string>("Testcase1", "a");
    dispatcher.call<std::string, std::string>("Testcase2", "b", "c");
    
    return 0;
}

