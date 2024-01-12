#include "WriteCgiEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

WriteCgiEvent::WriteCgiEvent(const Server &server, int clientSocket, int socket, std::string message)
    : AEvent(server, clientSocket), mMessage(message), mWriteSize(0), mFileSize(message.size()), mSocket(socket)
{
}

WriteCgiEvent::~WriteCgiEvent()
{
}

void WriteCgiEvent::handle()
{
    int n = write(mSocket, mMessage.c_str() + mWriteSize, mMessage.size() - mWriteSize);
    if (n == -1)
    {
        close(mSocket);
        perror("write fail");
        delete this;
        return;
    }

    mWriteSize += n;
    if (mWriteSize == mFileSize)
    {
        close(mSocket);
        delete this;
        return;
    }
}