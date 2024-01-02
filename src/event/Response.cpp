#include "Response.hpp"

Response::Response()
{
}

void Response::init(int httpStatusCode, int contentLength)
{
    if (httpStatusCode == 200)
    {
        mStartLine = "HTTP/1.1 " + std::to_string(httpStatusCode) + " " + "OK";
        mHead = "Content-Length: " + std::to_string(contentLength);
    }
}

const std::string &Response::getStartLine() const
{
    return mStartLine;
}

const std::string &Response::getHead() const
{
    return mHead;
}