#include "Request.hpp"
#include "util.hpp"
#include <sstream>

Request::Request() : mVersion("HTTP/1.1")
{
}

Request::~Request()
{
}

void Request::checkMethod(std::stringstream &ss)
{
    std::string method;
    ss >> method;
    if (method == "GET")
    {
        mMethod = E_GET;
    }
    else if (method == "POST")
    {
        mMethod = E_POST;
    }
    else if (method == "DELETE")
    {
        mMethod = E_DELETE;
    }
    else if (method == "HEAD" || method == "PUT" || method == "PATCH" || method == "OPTIONS" || method == "TRACE" ||
             method == "CONNECT")
    {
        mStatus = 501; // Not Implemented
    }
    else
    {
        mStatus = 400; // Bad Request
    }
}

void Request::checkPath(std::stringstream &ss)
{
    ss >> mPath;
    if (mPath == "")
    {
        mStatus = 400; // Bad Request
    }
}

void Request::checkHttpVersion(std::stringstream &ss)
{
    std::string version;
    ss >> version;
    if (version == "HTTP/1.0" || version == "HTTP/1.1")
    {
        mVersion = "HTTP/1.1"; // 1.0은 conectless이긴 함.
    }
    else if (version == "HTTP/2.0" || version == "HTTP/3.0")
    {
        mStatus = 501; // Not Implement
    }
    else
    {
        mStatus = 400; // Bad Request
    }
}

void Request::parseStartLine(std::string &buffer)
{
    size_t pos;
    if ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::stringstream ss(buffer.substr(0, pos));
        checkMethod(ss);
        checkPath(ss);
        checkHttpVersion(ss);
        buffer = buffer.substr(pos + 2);
        mRequestLine = E_REQUEST_HEADER;
    }
}

int Request::storeHeaderLine(const std::string &line)
{
    std::string headerKey;
    std::string headerVal;
    size_t pos = line.find(':');
    if (pos == std::string::npos)
    {
        mStatus = 400; // Bad Request
        return -1;
    }

    headerKey = trim(line.substr(0, pos));
    if (mHeaders.find(headerKey) != mHeaders.end())
    {
        mStatus = 400; // Bad Request
        return -1;
    }

    headerVal = trim(line.substr(pos + 1));
    if (headerVal.size() == 0)
    {
        mStatus = 400; // Bad Request
        return -1;
    }

    mHeaders[headerKey] = headerVal;
    return 0;
}

int Request::storeHeaderMap(std::string buffer)
{
    std::string line;

    size_t pos = 0;
    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        if (storeHeaderLine(buffer.substr(0, pos)))
            return -1;
        buffer = buffer.substr(pos + 2);
    }
    return 0;
}

void Request::parseRequestHeader(std::string &buffer)
{
    size_t pos = 0;
    if ((pos = buffer.find("\r\n\r\n")) != std::string::npos)
    {
        if (storeHeaderMap(buffer.substr(0, pos + 2)) == -1)
            return;
        if (mHeaders.find("Host") == mHeaders.end())
        {
            mStatus = 400; // Bad Request
            return;
        }
        buffer = buffer.substr(pos + 4);
        mRequestLine = E_REQUEST_CONTENTS;
    }
}

void Request::parseRequestContent(std::string &buffer)
{
    mContent += buffer;
    buffer = "";
}

void Request::parse(std::string &buffer)
{
    if (mRequestLine == E_START_LINE)
    {
        parseStartLine(buffer);
    }
    if (mRequestLine == E_REQUEST_HEADER)
    {
        parseRequestHeader(buffer);
    }
    if (mRequestLine == E_REQUEST_CONTENTS)
    {
        parseRequestContent(buffer);
    }
}

int Request::getStatus() const
{
    return mStatus;
}

void Request::clear()
{
    mRequestLine = E_START_LINE;
    mPath = "";
    mHeaders.clear();
    mContent = "";
    mStatus = 0;
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
    return mHeaders;
}

const std::string &Request::getPath() const
{
    return mPath;
}