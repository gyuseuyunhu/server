#include "Response.hpp"
#include "HttpStatusInfos.hpp"
#include <sstream>

Response::Response()
{
}

void Response::init(int httpStatusCode, int contentLength)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << httpStatusCode << " " << HttpStatusInfos::getHttpReason(httpStatusCode);
    mStartLine = oss.str();

    oss.str("");
    oss.clear();
    oss << "Content-Length: " << contentLength;
    mHead = oss.str();
}

void Response::addHead(const std::string &key, const std::string &value)
{
    mHead += CRLF + key + ": " + value;
}

const std::string &Response::getStartLine() const
{
    return mStartLine;
}

const std::string &Response::getHead() const
{
    return mHead;
}