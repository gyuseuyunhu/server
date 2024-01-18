#include "ReadCgiEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "WriteEvent.hpp"
#include "util.hpp"
#include <unistd.h>

ReadCgiEvent::ReadCgiEvent(const Server &server, int clientSocket, int fd)
    : AEvent(server, clientSocket), mFd(fd), mIsError(false)
{
}

bool ReadCgiEvent::setReponse(const std::string &line)
{
    std::string headerKey;
    std::string headerVal;
    size_t pos = line.find(':');
    if (pos == std::string::npos)
    {
        return false;
    }

    headerKey = line.substr(0, pos);
    headerVal = trim(line.substr(pos + 1));
    if (caseInsensitiveMatch(headerKey, "Status") == true)
    {
        mResponse.setStartLine(headerVal);
    }
    else
    {
        mResponse.addHead(headerKey, headerVal);
    }
    return true;
}

void ReadCgiEvent::handle()
{
    int n = read(mFd, mBuffer, BUFFER_SIZE);
    if (n < 0)
    {
        Kqueue::deleteEvent(mFd, EVFILT_TIMER);
        close(mFd);
        delete this;
        return;
    }
    mStringBuffer.append(mBuffer, n);
    if (n == 0)
    {
        size_t pos = mStringBuffer.find("\r\n");
        if (pos != std::string::npos)
        {
            while (pos != 0)
            {
                if (setReponse(mStringBuffer.substr(0, pos)) == false)
                {
                    mIsError = true;
                    break;
                }
                mStringBuffer.erase(0, pos + 2);
                pos = mStringBuffer.find("\r\n");
                if (pos == std::string::npos)
                {
                    mIsError = true;
                    break;
                }
            }
            if (mIsError == false)
            {
                mStringBuffer.erase(0, pos + 2);
            }
        }

        if (mIsError == true || mResponse.getStartLine().empty())
        {
            const std::string &errorPage = HttpStatusInfos::getHttpErrorPage(500);
            mResponse.setStartLine(500);
            mResponse.addHead("Content-type", "text/html");
            mResponse.addHead("Content-Length", errorPage.size());
            mResponse.setBody(errorPage);
        }
        else
        {
            mResponse.addHead("Content-Length", mStringBuffer.size());
            mResponse.setBody(mStringBuffer);
        }
        struct kevent newEvent;
        EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
        Kqueue::addEvent(newEvent);
        Kqueue::deleteEvent(mFd, EVFILT_TIMER);
        close(mFd);

        delete this;
    }
}

void ReadCgiEvent::timer()
{
    const std::string &errorPage = HttpStatusInfos::getHttpErrorPage(500);
    mResponse.setStartLine(500);
    mResponse.addHead("Content-type", "text/html");
    mResponse.addHead("Content-Length", errorPage.size());
    mResponse.setBody(errorPage);
    struct kevent newEvent;
    EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
    Kqueue::addEvent(newEvent);

    Kqueue::deleteEvent(mFd, EVFILT_TIMER);
    close(mFd);
    delete this;
}