#include "Response.hpp"
#include "HttpStatusInfos.hpp"
#include <sstream>

Response::Response() : mConnectionStatus(KEEP_ALIVE)
{
}

void Response::setStartLine(int httpStatusCode)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << httpStatusCode << " " << HttpStatusInfos::getHttpReason(httpStatusCode);
    mStartLine = oss.str();
}

void Response::setStartLine(const std::string &startLine)
{
    mStartLine = "HTTP/1.1 " + startLine;
}

void Response::addHead(const std::string &key, const int value)
{
    std::stringstream ss;
    ss << value;
    mHead += key + ": " + ss.str() + CRLF;
}

void Response::addHead(const std::string &key, const std::string &value)
{
    mHead += key + ": " + value + CRLF;
}

void Response::setBody(const std::string &body)
{
    mBody = body;
}

const std::string Response::toStr() const
{
    assert(mStartLine.size() != 0);
    assert(mHead.size() != 0);
    return mStartLine + CRLF + mHead + CRLF + mBody;
}

const std::string &Response::getStartLine() const
{
    return mStartLine;
}

const std::string &Response::getHead() const
{
    return mHead;
}

eConnectionStatus Response::getConnectionStatus() const
{
    return mConnectionStatus;
}

void Response::setConnectionClose()
{
    mConnectionStatus = CONNECTION_CLOSE;
    addHead("Connection", "close");
}