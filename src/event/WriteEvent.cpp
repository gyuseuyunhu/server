#include "WriteEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

WriteEvent::WriteEvent(const Server &server, int clientSocket, std::string message, int status)
    : AEvent(server, clientSocket), mMessage(message), mWriteSize(0), mResponseSize(message.size()), mStatus(status)
{
}

WriteEvent::~WriteEvent()
{
}

void WriteEvent::handle()
{
    int n;

    n = write(mClientSocket, mMessage.c_str(), mMessage.size() < BUFFER_SIZE ? mMessage.size() : BUFFER_SIZE);
    if (n == -1)
    {
        close(mClientSocket);
        delete this;
        return;
    }

    mWriteSize += n;
    if (mWriteSize == mResponseSize)
    {
        if (mStatus == 400 || mStatus == 501)
        {
            close(mClientSocket);
            delete this;
            return;
        }
        Kqueue::deleteEvent(mClientSocket, EVFILT_WRITE);
        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_READ, EV_ADD, 0, 0, new ReadRequestEvent(mServer, mClientSocket));
        Kqueue::addEvent(newEvent);
        delete this;
        return;
    }

    mMessage = mMessage.substr(n);
}