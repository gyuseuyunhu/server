#include "WriteEvent.hpp"

WriteEvent::WriteEvent(const Server &server, int clientSocket, std::string message)
    : AEvent(server, clientSocket), mMessage(message), mWriteSize(0), mResponseSize(message.size())
{
}

WriteEvent::~WriteEvent()
{
}

int WriteEvent::handle()
{
    int n;

    n = write(mClientSocket, mMessage.c_str(), mMessage.size());
    mWriteSize += n;
    if (n == -1)
    {
        // todo 모름
        return EVENT_CONTINUE;
    }
    else if (mWriteSize == mResponseSize)
    {

        return EVENT_FINISH;
    }
    else
    {
        mMessage.substr(n, mMessage.size());
        return EVENT_CONTINUE;
    }
}