#include "ReadFileEvent.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
#include <sys/event.h>
#include <unistd.h>

ReadFileEvent::ReadFileEvent(const Server &server, int clientSocket, int fileFd, int fileSize, int httpStatusCode)
    : AEvent(server, clientSocket), mFileFd(fileFd), mFileSize(fileSize), mReadSize(0), mHttpStatusCode(httpStatusCode)
{
}

ReadFileEvent::~ReadFileEvent()
{
}

int ReadFileEvent::handle()
{
    int n = read(mFileFd, mBuffer, BUFFER_SIZE);

    if (n < 0) // 읽기를 실패 -> client에 500번대를 응답
    {
        // 처리 필요 autoindex 또는 301 또는 404
        // write
        return EVENT_FINISH;
    }
    mReadSize += n;
    mBody.append(mBuffer, n);
    if (mReadSize == mFileSize)
    {
        mResponse.init(mHttpStatusCode, mReadSize);
        std::string message;
        message = mResponse.getStartLine() + "\r\n" + mResponse.getHead() + "\r\n\r\n" + mBody;

        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mClientSocket, message));
        Kqueue::addEvent(newEvent);
        return EVENT_FINISH;
    }
    else
    {
        return EVENT_CONTINUE;
    }
}
