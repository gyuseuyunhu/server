#include "WriteEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

// mResponseSize(response.toStr().size()) 로 한 이유는 소켓에 write() 할 string의 길이가 필요하기 때문
WriteEvent::WriteEvent(const Server &server, const Response &response, int clientSocket)
    : AEvent(server, response, clientSocket), mMessage(response.toStr()), mWriteSize(0),
      mResponseSize(response.toStr().size())
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