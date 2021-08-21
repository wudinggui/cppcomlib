#include <iostream>
#include <string>

class BaseEventSource;
class BaseEvent {
public:
    BaseEvent(BaseEventSource* source) {
        mSource = source;
    }
    BaseEventSource* mSource;
};

class BaseEventListner {
public:
    virtual void OnNoitfy(BaseEvent* event) = 0;
    BaseEventSource* mSource;
};

class BaseEventSource {
public:
    BaseEventSource(std::string name) : mName(name)
    {}
    void BindEventListner(BaseEventListner* listner) {
        mListner = listner;
        mListner->mSource = this;
    }
    std::string mName;
    BaseEventListner* mListner;
};

//====================================
class ServiceEvent : public BaseEvent {
public:
    enum class SERVICE_EVENT_TYPE {
        SUCCESS = 1,
        FAILED
    };

    ServiceEvent(BaseEventSource* source)
    : BaseEvent(source) {
    }

    SERVICE_EVENT_TYPE mType;
};

class ServiceListner : public BaseEventListner {
public:
    void OnNoitfy(BaseEvent* event) {
        ServiceEvent* ev = (ServiceEvent*)event;
        switch (ev->mType) {
        case ServiceEvent::SERVICE_EVENT_TYPE::SUCCESS:
            std::cout << "ServiceListner recv SUCCESS from " << ev->mSource->mName << std::endl;
            break;
        case ServiceEvent::SERVICE_EVENT_TYPE::FAILED:
            std::cout << "ServiceListner recv FAILED from " << ev->mSource->mName << std::endl;
            break;
        default:
            break;
        }
    }
};

class Service : public BaseEventSource {
public:
    Service() : BaseEventSource("Service") {
    }

    void DoSomething() {
        ServiceEvent ev(this);
        ev.mType = ServiceEvent::SERVICE_EVENT_TYPE::FAILED;

        mListner->OnNoitfy(&ev);
    }
};

int main()
{
    Service app;
    ServiceListner listner;
    app.BindEventListner(&listner);

    app.DoSomething();

    return 0;
}
