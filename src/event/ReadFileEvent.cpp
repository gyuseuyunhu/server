#include "ReadFileEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
#include <sys/event.h>
#include <unistd.h>

ReadFileEvent::ReadFileEvent(const Server &server, const Response &response, int clientSocket, int fileFd, int fileSize)
    : AEvent(server, response, clientSocket), mFileFd(fileFd), mFileSize(fileSize), mReadSize(0), mBody("")
{
}
ReadFileEvent::~ReadFileEvent()
{
}

void ReadFileEvent::handle()
{
    int n = read(mFileFd, mBuffer, BUFFER_SIZE);

    if (n < 0)
    {
        close(mFileFd);
        delete this;
        return;
    }
    mReadSize += n;
    mBody.append(mBuffer, n);
    if (mReadSize == mFileSize)
    {
        close(mFileFd);
        mResponse.setBody(mBody);

        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
        Kqueue::addEvent(newEvent);
        delete this;
    }
}
