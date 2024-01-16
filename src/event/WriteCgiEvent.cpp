#include "WriteCgiEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"

WriteCgiEvent::WriteCgiEvent(const Server &server, int clientSocket, int fd, std::string message)
    : AEvent(server, clientSocket), mMessage(message), mWriteSize(0), mFileSize(message.size()), mFd(fd)
{
}

WriteCgiEvent::~WriteCgiEvent()
{
}

void WriteCgiEvent::handle()
{
    int n = write(mFd, mMessage.c_str() + mWriteSize, mMessage.size() - mWriteSize);
    if (n == -1)
    {
        close(mFd);
        perror("write fail");
        delete this;
        return;
    }

    mWriteSize += n;
    if (mWriteSize == mFileSize)
    {
        close(mFd);
        delete this;
        return;
    }
}

void WriteCgiEvent::timer()
{
    close(mFd);
    delete this;
}