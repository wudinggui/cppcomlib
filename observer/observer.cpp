#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

typedef  std::function<int (std::string)> Func;
class Publisher
{
public:

    void register_handler(const std::string& name, const Func& f)
    {
        if (invokers_.find(name) != invokers_.end())
            throw std::invalid_argument("this key has already exist!");

        invokers_.emplace(name, f);
    }
   
    void unregister_handler(const std::string& name)
    {
        if (invokers_.find(name) == invokers_.end())
            return;
        
        invokers_.erase(name);
    }

    template<typename... Args>
    void notify(Args&&... args)
    {
        for (auto& it: invokers_)
        {
            auto invoker = it.second;
            if (invoker)
                invoker(args...);
        }
    }

private:
    std::unordered_map<std::string, Func> invokers_;
};

class Subscriber
{
public:
    Subscriber(const std::string& name, std::shared_ptr<Publisher>& pub)
    {
        name_ = name;
        pub_ = pub;
        pub_->register_handler(name_, std::bind(&Subscriber::update, this, std::placeholders::_1));
    }

private:
    int update(std::string a)
    {
        std::cout <<  "update("<< a <<")" << std::endl;
        return 0;
    }

    std::string name_;
    std::shared_ptr<Publisher> pub_;
};

int main()
{
    std::shared_ptr<Publisher> pub(new Publisher());
    
    Subscriber sub1("sub1", pub);
    Subscriber sub2("sub2", pub);
    
    pub->notify<std::string>("status");
    
    return 0;
}