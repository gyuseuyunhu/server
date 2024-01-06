#include "ReadFileEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
#include <sys/event.h>
#include <unistd.h>

ReadFileEvent::ReadFileEvent(const Server &server, int clientSocket, int fileFd, int fileSize, int httpStatusCode)
    : AEvent(server, clientSocket), mFileFd(fileFd), mFileSize(fileSize), mReadSize(0), mHttpStatusCode(httpStatusCode),
      mBody("")
{
}

ReadFileEvent::~ReadFileEvent()
{
}

int ReadFileEvent::handle()
{
    int n = read(mFileFd, mBuffer, BUFFER_SIZE);

    if (n < 0)
    {
        // 처리 필요 autoindex 또는 301 또는 404
        // write
        close(mFileFd);
        delete this;
        return EVENT_FINISH;
    }
    mReadSize += n;
    mBody.append(mBuffer, n);
    if (mReadSize == mFileSize)
    {
        close(mFileFd);
        mResponse.init(mHttpStatusCode, mReadSize);
        std::string message = mResponse.getStartLine() + CRLF + mResponse.getHead() + CRLF CRLF + mBody;

        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mClientSocket, message));
        Kqueue::addEvent(newEvent);
        delete this;
        return EVENT_FINISH;
    }
    else
    {
        return EVENT_CONTINUE;
    }
}
