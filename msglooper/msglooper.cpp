#include "msglooper.hpp"

IMessageHandler *IMessage::GetHandler() const {
    return mHandler;
}

void IMessage::Post()
{
    mHandler->mLooper->PostMessage(this);
}

int IMessage::PostAndWait(IMessage* response)
{
    mHandler->mLooper->PostMessageAndWait(this, response);
    return 0;
}

uint32_t IMessage::GetSequence() const
{
    return mSequence;
}

void IMessage::SetSequence(uint32_t seq)
{
    mSequence = seq;
}

int IMessage::GetCode() const
{
    return mCode;
}

void *IMessage::GetThis() const
{
    return (void*)this;
}

void MessageLooper::RegMessageHandler(IMessageHandler *handler)
{
    handler->mLooper = this;
    mHandlers.insert(handler);
}

void MessageLooper::PostMessage(IMessage *msg)
{
    mMessageQueue.push(msg);
}

int MessageLooper::PostMessageAndWait(IMessage *msg, IMessage *response)
{
    mMessageQueue.push(msg);

    // create a promise and wait for the future things to be done
    std::promise<IMessage *> prom;
    std::future<IMessage *>  fut = prom.get_future();

	mRespones[msg->GetSequence()] = std::move(prom);

	std::chrono::milliseconds timeout(5);
    while (fut.wait_for(timeout) == std::future_status::timeout) {
        std::cout << "Time out" << std::endl;
        return -1;
    }

    auto ret = fut.get();
    if (ret == nullptr) {
        std::cout << "response == nullptr" << std::endl;
        return -1;
    }
    response->copy(ret);

    // remove the promise
    delete ret;
    auto it = mRespones.find(msg->GetSequence());
	if (it != mRespones.end()) {
	    mRespones.erase(it);
	}
    

	return 0;
}

int MessageLooper::ReplyMessage(IMessage *response)
{
    auto it = mRespones.find(response->GetSequence());
	if (it == mRespones.end()) {
	    return -1;
	}

	std::promise<IMessage *>& prom = it->second;
    IMessage *copy = response->dup();
    prom.set_value(copy);
    return 0;
}

void MessageLooper::Start()
{
    mStart = true;
    while (mStart) {
        std::cout << "wait" << std::endl;
        IMessage *msg = nullptr;
        mMessageQueue.wait_and_pop(msg);
        std::cout << "mMessageQueue.pop" << std::endl;
        if (msg) {
            IMessageHandler *handler = msg->GetHandler();
            if (mHandlers.find(handler) != mHandlers.end()) {
                handler->OnRecieve(msg);
            }
        }
    }
    std::cout << "looper exit" << std::endl;
}

void MessageLooper::Stop()
{
    mStart = false;
    mMessageQueue.stop_wait_and_pop();
    std::cout << "stop looper" << std::endl;
}

