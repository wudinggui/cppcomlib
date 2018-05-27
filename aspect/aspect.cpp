#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <utility>

typedef  std::function<int (std::string)> Func;
typedef  std::vector<std::pair<std::string, Func>> Vecfuncs;
class Aspect
{
public:
    void register_handler(const std::string& name, const Func& bf, const Func& af)
    {
        for (auto& it: before_invokers_)
        {
            if (it.first == name)
                throw std::invalid_argument("this key has already exist!");
        }
        
        for (auto& it: after_invokers_)
        {
            if (it.first == name)
                throw std::invalid_argument("this key has already exist!");
        }    
        
        before_invokers_.push_back(std::make_pair(name, bf));
        after_invokers_.push_back(std::make_pair(name, af));
    }

    template<typename... Args>
    void invoke_handler(const Func& f, Args... args)
    {
        notify(before_invokers_, args...);
        if (f)
        {
            f(args...);
        }
        notify(after_invokers_, args...);
    }

    template<typename... Args>
    void notify(Vecfuncs invokers, Args&&... args)
    {
        for (auto& it: invokers)
        {
            auto invoker = it.second;
            if (invoker)
                invoker(args...);
        }
    }

private:
    Vecfuncs before_invokers_;
    Vecfuncs after_invokers_;
};

class Server : public Aspect
{
public:
    int dothings(std::string x)
    {
        std::cout << "dothings: " << x << std::endl;
        return 0;
    }
};

struct Log
{
    int log(std::string x)
    {
        std::cout << "log: " << x << std::endl;
        return 0;
    }
};

struct Auth
{
    int auth(std::string x)
    {
        std::cout << "auth: " << x << std::endl;
        return 0;
    }
};

int main()
{
    Log logger;
    Auth authern;
    std::shared_ptr<Server> srv(new Server());
    
    Func loghandler = std::bind(&Log::log, &logger, std::placeholders::_1);
    Func authhandler = std::bind(&Auth::auth, &authern, std::placeholders::_1);
    srv->register_handler("log", loghandler, loghandler);
    srv->register_handler("auth", authhandler, nullptr);
    
    Func srchandler = std::bind(&Server::dothings, srv, std::placeholders::_1);
    srv->invoke_handler<std::string>(srchandler,"foo");
    
    return 0;
}