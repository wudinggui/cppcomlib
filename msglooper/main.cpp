
#include <iostream>
#include <thread>
#include "msglooper.hpp"

class MessageA : public IMessage {
public:
    MessageA() = default;
    virtual ~MessageA() {
        std::cout << "~MessageA() " << std::endl;
    }
    MessageA(int code, IMessageHandler* hander)
    : IMessage(code, hander) {
    }
    enum MessageACode {
        SUCCESS = 1,
        FAILED,
        SYCNCALL,
    };

    void* GetThis() const
    {
        return (void* )this;
    }

    int x = 1;
};

class MessageARespone : public MessageA {
    public:
    MessageARespone() = default;
    virtual ~MessageARespone() {
        std::cout << "~MessageARespone() " << std::endl;
    }
    MessageARespone(int code, uint32_t seq, IMessageHandler* hander)
    : MessageA(code, hander) {
        SetSequence(seq);
    }

    virtual void copy(const IMessage* other) {
        x = ((MessageARespone *)(other->GetThis()))->x;
    }

    virtual IMessage* dup() {
        MessageARespone* instance = new MessageARespone(
                                            this->GetCode(), 
                                            this->GetSequence(), 
                                            this->GetHandler());
        instance->x = x;
        return instance;
    }
};

class HandlerA : public IMessageHandler {
public:
    virtual ~HandlerA() {
        std::cout << "~HandlerA() " << std::endl;
    }
    void OnRecieve(IMessage* msg) {
        switch (msg->GetCode()) {
        case MessageA::SUCCESS:
            std::cout << "HandlerA recv SUCCESS " << ((MessageA *)(msg->GetThis()))->x << std::endl;
            break;
        case MessageA::FAILED:
            std::cout << "HandlerA recv FAILED " << ((MessageA *)(msg->GetThis()))->x << std::endl;
            break;
        case MessageA::SYCNCALL: {
                std::cout << "HandlerA recv SYCNCALL " << ((MessageA *)(msg->GetThis()))->x << std::endl;
                MessageARespone reply(MessageA::SUCCESS, msg->GetSequence(), this);
                reply.x = 3;
                this->mLooper->ReplyMessage(&reply);
                std::cout << "ReplyMessage OK" << std::endl;
            }
            break;
        default:
            break;
        }
    }
};

int main()
{
    MessageLooper loop;
    HandlerA handler;
    loop.RegMessageHandler(&handler);
    std::thread t(std::bind(&MessageLooper::Start, &loop));

    MessageA msga(MessageA::FAILED, &handler);
    msga.x = 1;
    msga.Post();

    MessageA msgb(MessageA::SUCCESS, &handler);
    msgb.x = 2;
    msgb.Post(); 

    MessageA msgc(MessageA::SYCNCALL, &handler);
    msgc.x = 3;
    MessageARespone response;
    int ret = msgc.PostAndWait(&response);
    std::cout << "PostAndWait ret = " << ret << ", x = "<< response.x << std::endl;

    sleep(1);
    loop.Stop();
    sleep(1);
    return 0;
}
