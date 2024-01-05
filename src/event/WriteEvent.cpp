#include "WriteEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

#define BUFFER_SIZE 3000

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

    // std::cout << "write event start" << std::endl;
    n = write(mClientSocket, mMessage.c_str(), mMessage.size() < BUFFER_SIZE ? mMessage.size() : BUFFER_SIZE);
    // std::cout << "write event end" << std::endl;
    mWriteSize += n;
    if (n == -1)
    {
        // todo 모름
        close(mClientSocket);
        delete this;
        return EVENT_CONTINUE;
    }
    else if (mWriteSize == mResponseSize)
    {
        Kqueue::deleteEvent(mClientSocket, EVFILT_WRITE);
        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_READ, EV_ADD, 0, 0, new ReadRequestEvent(mServer, mClientSocket));
        Kqueue::addEvent(newEvent);
        delete this;
        return EVENT_FINISH;
    }
    else
    {
        mMessage.substr(n);
        return EVENT_CONTINUE;
    }
}