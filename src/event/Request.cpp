#include "Request.hpp"
#include "util.hpp"
#include <sstream>

Request::Request()
    : mMethod(E_GET), mPath(""), mVersion("HTTP/1.1"), mHost(""), mContent(""), mRequestLine(E_START_LINE), mStatus(0)
{
}

Request::~Request()
{
}

int Request::checkMethod(std::stringstream &ss)
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
        mMethod = E_NOT_IMPLEMENT;
        return -1;
    }
    else
    {
        mStatus = 400; // Bad Request
        mMethod = E_BAD_REQUEST;
        return -1;
    }
    return 0;
}

int Request::checkPath(std::stringstream &ss)
{
    ss >> mPath;
    if (mPath == "")
    {
        mStatus = 400; // Bad Request
        return -1;
    }
    if (mPath[0] != '/')
    {
        mPath = "/" + mPath;
    }
    return 0;
}

int Request::checkHttpVersion(std::stringstream &ss)
{
    std::string version;
    ss >> version;
    if (version == "HTTP/1.0" || version == "HTTP/1.1")
    {
        mVersion = "HTTP/1.1"; // 1.0은 conectless이긴 함.
        return 0;
    }
    else if (version == "HTTP/2.0" || version == "HTTP/3.0")
    {
        mStatus = 501; // Not Implement
        return -1;
    }
    else
    {
        mStatus = 400; // Bad Request
        return -1;
    }
}

void Request::parseStartLine(std::string &buffer)
{
    size_t pos;
    if ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::stringstream ss(buffer.substr(0, pos));
        if (checkMethod(ss) == -1 || checkPath(ss) == -1 || checkHttpVersion(ss) == -1)
            return;
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
        mHost = mHeaders["Host"];
        mStatus = 200;
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

const std::string &Request::getHost() const
{
    return mHost;
}

const std::string &Request::getPath() const
{
    return mPath;
}