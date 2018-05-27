#include <iostream>
#include<string>
#include<unordered_map>
#include<memory>
#include<functional>
#include<boost/any.hpp>

class Autofactory
{
public:
    Autofactory() {};
    Autofactory(const Autofactory&) = delete;
    Autofactory(Autofactory&&) = delete;

    inline static Autofactory& get()
    {
        static Autofactory instance;
        return instance;
    }
    
    template<class Base, typename Derived>
    struct register_t
    {
        register_t(const std::string& name)
        {
            Autofactory::get().register_type<Base, Derived>(name);
        }
    };
    
    template<class Base, typename Derived>
    void register_type(const std::string& name)
    {
        if (invokers_.find(name) != invokers_.end())
            throw std::invalid_argument("this key has already exist!");

        std::function<Base* ()> function = []( ){ return static_cast<Base*>(new Derived()); };
        boost::any a(function);
        invokers_.emplace(name, a);
    }

    template<class T>
    T* produce(const std::string& name)
    {
        if (invokers_.find(name) == invokers_.end())
            return nullptr;

        boost::any resolver = invokers_[name];
        std::function<T* ()> function = boost::any_cast<std::function<T* ()>>(resolver);
        
        return function();
    }

    template<class T>
    std::shared_ptr<T> produce_ptr(const std::string& name)
    {
        T* t = produce<T>(name);
        return std::shared_ptr<T>(t);
    }

private:
    std::unordered_map<std::string, boost::any> invokers_;
};

#define REGISTER_TYPE(name, Base, Derived) \
    static Autofactory::register_t<Base, Derived> Reg_##Derived##Type(name);
    
    
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

REGISTER_TYPE("Bus", ICar, Bus)

struct Truck : public ICar
{
             Truck(){std::cout <<  "Truck " << std::endl;}
    virtual ~Truck(){std::cout <<  "~Truck" << std::endl;}
};
REGISTER_TYPE("Truck", ICar, Truck)

int main()
{
    auto bus = Autofactory::get().produce_ptr<ICar>("Bus");
    auto truck = Autofactory::get().produce_ptr<ICar>("Truck");
    
    return 0;
}

