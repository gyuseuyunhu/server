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

bool ReadCgiEvent::setResponse(const std::string &line)
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
        Kqueue::deleteEvent(this, EVFILT_TIMER);
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
                if (setResponse(mStringBuffer.substr(0, pos)) == false)
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
        Kqueue::addEvent(new WriteEvent(mServer, mResponse, mClientSocket), EVFILT_WRITE);
        Kqueue::deleteEvent(this, EVFILT_TIMER);
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
    Kqueue::addEvent(new WriteEvent(mServer, mResponse, mClientSocket), EVFILT_WRITE);

    Kqueue::deleteEvent(this, EVFILT_TIMER);
    close(mFd);
    delete this;
}

int ReadCgiEvent::getFd() const
{
    return mFd;
}