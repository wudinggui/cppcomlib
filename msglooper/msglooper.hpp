#ifndef __MESSAGEE_LOOPER_H
#define __MESSAGEE_LOOPER_H
#include <string>
#include <set>
#include <unordered_map>
#include <future>
#include "thread_save_queue.hpp"

class IMessageHandler;
class MessageLooper;
class IMessage {
public:
    IMessage()
    {}
    IMessage(int code, IMessageHandler *handler) :
        mCode(code), mHandler(handler)
    {
    }
    virtual ~IMessage() = default;
    IMessageHandler *GetHandler() const;
    void Post();
    int PostAndWait(IMessage* response);
    virtual int GetCode() const;
    virtual void* GetThis() const;
    virtual uint32_t GetSequence() const;
    virtual void SetSequence(uint32_t seq);
    virtual void copy(const IMessage* other) {
        mCode = other->GetCode();
        mSequence = other->GetSequence();
        mHandler = other->GetHandler();
    }

    virtual IMessage* dup() {
        return this;
    }

private:
    int mCode;
    uint32_t mSequence;
    IMessageHandler *mHandler;
};

class IMessageHandler {
public:
    IMessageHandler() = default;
    virtual ~IMessageHandler() = default;
    virtual void OnRecieve(IMessage *msg) = 0;
    MessageLooper *mLooper;
};

class MessageLooper {
public:
    MessageLooper()
    {
    }
    virtual ~MessageLooper() = default;
    void RegMessageHandler(IMessageHandler *handler);
    void PostMessage(IMessage *msg);
    int PostMessageAndWait(IMessage *msg, IMessage *response);
    int ReplyMessage(IMessage *response);
    void Start();
    void Stop();

private:
    bool mStart = false;
    std::set<IMessageHandler *> mHandlers;
    std::unordered_map<uint32_t, std::promise<IMessage *> > mRespones;
    thread_save_queue<IMessage *>  mMessageQueue;
};

#endif
