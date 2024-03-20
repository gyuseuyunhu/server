#include "ReadFileEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
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
#ifdef __linux__
    while (true)
    {
        int n = read(mFileFd, mBuffer, BUFFER_SIZE);
        if (n < 0)
        {
            Kqueue::deleteEvent(this, EVFILT_TIMER);
            close(mFileFd);
            delete this;
            return;
        }
        mReadSize += n;
        mBody.append(mBuffer, n);
        if (mReadSize == mFileSize)
        {
            Kqueue::deleteEvent(this, EVFILT_TIMER);
            close(mFileFd);
            mResponse.setBody(mBody);
            Kqueue::addEvent(new WriteEvent(mServer, mResponse, mClientSocket), EVFILT_WRITE);
            delete this;
            return;
        }
    }
#endif
    int n = read(mFileFd, mBuffer, BUFFER_SIZE);
    if (n < 0)
    {
        Kqueue::deleteEvent(this, EVFILT_TIMER);
        close(mFileFd);
        delete this;
        return;
    }
    mReadSize += n;
    mBody.append(mBuffer, n);

    if (mReadSize == mFileSize)
    {
        Kqueue::deleteEvent(this, EVFILT_TIMER);
        close(mFileFd);
        mResponse.setBody(mBody);
        Kqueue::addEvent(new WriteEvent(mServer, mResponse, mClientSocket), EVFILT_WRITE);
        delete this;
    }
}

void ReadFileEvent::timer()
{
    const std::string &errorPage = HttpStatusInfos::getHttpErrorPage(408);
    mResponse.setStartLine(408);
    mResponse.addHead("Content-type", "text/html");
    mResponse.addHead("Content-Length", errorPage.size());
    mResponse.setBody(errorPage);
    Kqueue::addEvent(new WriteEvent(mServer, mResponse, mClientSocket), EVFILT_WRITE);

    Kqueue::deleteEvent(this, EVFILT_TIMER);
    close(mFileFd);
    delete this;
}

int ReadFileEvent::getFd() const
{
    return mFileFd;
}