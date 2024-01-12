#include "ReadCgiEvent.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
#include <unistd.h>

ReadCgiEvent::ReadCgiEvent(const Server &server, int clientSocket, int socket)
    : AEvent(server, clientSocket), mSocket(socket)
{
    mStringBuffer.reserve(100000800);
}

void ReadCgiEvent::handle()
{
    int n = read(mSocket, mBuffer, BUFFER_SIZE);
    if (n < 0)
    {
        close(mSocket);
        delete this;
        return;
    }
    mStringBuffer.append(mBuffer, n);
    if (n == 0)
    {
        size_t pos = mStringBuffer.find("\r\n\r\n");
        mStringBuffer.erase(0, pos + 4);
        mResponse.setStartLine(200);
        mResponse.addHead("Content-Length", mStringBuffer.size());
        mResponse.addHead("Content-Type", "text/html");
        mResponse.setBody(mStringBuffer);

        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
        Kqueue::addEvent(newEvent);
        close(mSocket);

        delete this;
    }
}