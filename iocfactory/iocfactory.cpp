#include <iostream>
#include<string>
#include<unordered_map>
#include<memory>
#include<functional>
#include<boost/any.hpp>

class Iocfactory
{
public:
    Iocfactory(){}
    ~Iocfactory(){}

    template<class Base, typename Derived, typename... Args>
    void register_type(const std::string& name)
    {
        if (invokers_.find(name) != invokers_.end())
            throw std::invalid_argument("this key has already exist!");

        std::function<Base* (Args...)> function = [](Args... args){ return static_cast<Base*>(new Derived(args...)); };
        boost::any a(function);
        invokers_.emplace(name, a);
    }

    template<class T, typename... Args>
    T* produce(const std::string& name, Args... args)
    {
        if (invokers_.find(name) == invokers_.end())
            return nullptr;

        boost::any resolver = invokers_[name];
        std::function<T* (Args...)> function = boost::any_cast<std::function<T* (Args...)>>(resolver);
        
        return function(args...);
    }

    template<class T, typename... Args>
    std::shared_ptr<T> produce_ptr(const std::string& name, Args... args)
    {
        T* t = produce<T>(name, args...);
        return std::shared_ptr<T>(t);
    }

private:
    std::unordered_map<std::string, boost::any> invokers_;
};

struct ICar
{
             ICar(){std::cout <<  "ICar"  << std::endl;}
    virtual ~ICar(){std::cout <<  "~ICar" << std::endl;}
};

struct Bus : public ICar
{
             Bus(){std::cout <<  "Bus" << std::endl;}
    virtual ~Bus(){std::cout <<  "~Bus" << std::endl;}
};

struct Truck : public ICar
{
             Truck(std::string a){std::cout <<  "Truck " << a << std::endl;}
    virtual ~Truck(){std::cout <<  "~Truck" << std::endl;}
};

int main()
{
    Iocfactory iocfactory;
    
    iocfactory.register_type<ICar, Bus>("Bus");
    iocfactory.register_type<ICar, Truck, std::string>("Truck");  

    auto bus = iocfactory.produce_ptr<ICar>("Bus");
    auto truck = iocfactory.produce_ptr<ICar, std::string>("Truck", "test");
    return 0;
}

