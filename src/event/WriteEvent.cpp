#include "WriteEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

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

    n = write(mClientSocket, mMessage.c_str() + mWriteSize, mMessage.size() - mWriteSize);
    if (n == -1)
    {
        perror("writeEvent");
        close(mClientSocket);
        delete this;
        return;
    }

    mWriteSize += n;
    if (mWriteSize == mResponseSize)
    {
        if (mResponse.getConnectionStatus() == CONNECTION_CLOSE)
        {
            close(mClientSocket);
            delete this;
            return;
        }
        Kqueue::deleteEvent(this, EVFILT_WRITE);
        AEvent *event = new ReadRequestEvent(mServer, mClientSocket);
        Kqueue::addEvent(event, EVFILT_READ);
        Kqueue::addEvent(event, EVFILT_TIMER);
        delete this;
        return;
    }
}

int WriteEvent::getFd() const
{
    return mClientSocket;
}