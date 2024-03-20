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
        Kqueue::deleteEvent(this, EVFILT_TIMER);
        Kqueue::deleteEvent(this, EVFILT_WRITE);
        close(mFd);
        perror("write fail");
        delete this;
        return;
    }

    mWriteSize += n;
    if (mWriteSize == mFileSize)
    {
        Kqueue::deleteEvent(this, EVFILT_TIMER);
        Kqueue::deleteEvent(this, EVFILT_WRITE);
        close(mFd);
        delete this;
        return;
    }
}

void WriteCgiEvent::timer()
{
    Kqueue::deleteEvent(this, EVFILT_TIMER);
    close(mFd);
    delete this;
}

int WriteCgiEvent::getFd() const
{
    return mFd;
}