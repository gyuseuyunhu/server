#include "Request.hpp"
#include "HttpStatusInfos.hpp"
#include "util.hpp"
#include <iostream>
#include <sstream>

bool CaseInsensitiveCompare::operator()(const std::string &a, const std::string &b) const
{
    size_t minLength = std::min(a.length(), b.length());
    for (size_t i = 0; i < minLength; ++i)
    {
        if (std::tolower(a[i]) < std::tolower(b[i]))
        {
            return true;
        }
        if (std::tolower(a[i]) > std::tolower(b[i]))
        {
            return false;
        }
    }
    return a.length() < b.length();
}

Request::Request(const Server &server)
    : mServer(server), mMethod(E_GET), mPath(""), mVersion("HTTP/1.1"), mHost(""), mBody(""), mContentLength(0),
      mRequestLine(E_START_LINE), mStatus(0), mConnectionStatus(KEEP_ALIVE), mChunkedSize(NO_SIZE)
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
        mStatus = 405; // Not Allowed
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
    if (mPath.empty())
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
    if ((pos = buffer.find(CRLF)) != std::string::npos)
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

    headerKey = line.substr(0, pos);
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
    while ((pos = buffer.find(CRLF)) != std::string::npos)
    {
        if (storeHeaderLine(buffer.substr(0, pos)))
        {
            return -1;
        }
        buffer = buffer.substr(pos + 2);
    }
    return 0;
}

void Request::parseRequestHeader(std::string &buffer)
{
    size_t pos = 0;
    if ((pos = buffer.find(CRLF CRLF)) != std::string::npos)
    {
        if (storeHeaderMap(buffer.substr(0, pos + 2)) == -1)
        {
            mStatus = 400;
            return;
        }
        if (mHeaders.find("Host") == mHeaders.end())
        {
            mStatus = 400; // Bad Request
            return;
        }
        mHost = mHeaders["Host"];

        if (mHeaders.find("Connection") != mHeaders.end() && mHeaders["Connection"] == "close")
        {
            mConnectionStatus = CONNECTION_CLOSE;
        }

        const LocationBlock &lb = mServer.getLocationBlockForRequest(mHost, mPath);
        mClientMaxBodySize = lb.getClientMaxBodySize();
        mIsAllowedGet = lb.isAllowedGet();
        mIsAllowedPost = lb.isAllowedPost();
        mIsAllowedDelete = lb.isAllowedDelete();
        if ((mMethod == E_DELETE && mIsAllowedDelete == false) || (mMethod == E_GET && mIsAllowedGet == false) ||
            (mMethod == E_POST && mIsAllowedPost == false))
        {
            mStatus = 405;
            return;
        }
        if (checkChunkedData())
        {
            buffer = buffer.substr(pos + 4);
            mRequestLine = E_CHUNKED_CONTENTS;
            return;
        }
        if (mHeaders.find("Content-Length") != mHeaders.end())
        {
            std::stringstream ss;
            ss << mHeaders["Content-Length"];
            ss >> mContentLength;
            buffer = buffer.substr(pos + 4);
            mRequestLine = E_REQUEST_CONTENTS;
            return;
        }

        mStatus = 200;
    }
}

bool Request::checkChunkedData(void)
{
    std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator iter =
        mHeaders.find("Transfer-Encoding");
    if (iter != mHeaders.end())
    {
        if (iter->second == "chunked")
        {
            return true;
        }
    }
    return false;
}
void Request::parseChunkedContent(std::string &buffer)
{
    std::stringstream ss;
    size_t pos = 0;

    if (mChunkedSize == NO_SIZE && (pos = buffer.find(CRLF)) != std::string::npos)
    {
        std::string value = buffer.substr(0, pos);
        char *endptr;
        mChunkedSize = strtol(value.c_str(), &endptr, 16);
        if (endptr[0] != '\0')
        {
            mStatus = 400;
            return;
        }
        if (mChunkedSize == NO_SIZE)
        {
            if (mBody.size() == 0)
            {
                mStatus = 400;
                return;
            }
            mStatus = 200;
            return;
        }
        buffer = buffer.substr(pos + 2);
    }
    else if (mChunkedSize != NO_SIZE && (mChunkedSize + 2) <= buffer.size())
    {
        if (buffer.substr(mChunkedSize, 2) != CRLF)
        {
            mStatus = 400;
            return;
        }
        mBody += buffer.substr(0, mChunkedSize);
        if (mBody.size() > mClientMaxBodySize)
        {
            mStatus = 400;
            return;
        }
        buffer = buffer.substr(mChunkedSize + 2);
        mChunkedSize = NO_SIZE;
    }
}

void Request::parseRequestContent(std::string &buffer)
{
    mBody += buffer;
    buffer = "";
    if (mBody.size() == mContentLength)
    {
        mStatus = 200;
    }
    // ContentLength보다 더 많이 들어왔을 때
    else if (mBody.size() > mContentLength || mBody.size() > mClientMaxBodySize)
    {
        mStatus = 400;
    }
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
    if (mRequestLine == E_CHUNKED_CONTENTS)
    {
        parseChunkedContent(buffer);
    }
    // if (mStatus == 400 || mStatus == 501)
    // {
    //     mConnectionStatus = CONNECTION_CLOSE;
    // }
}

int Request::getStatus() const
{
    return mStatus;
}

const std::map<std::string, std::string, CaseInsensitiveCompare> &Request::getHeaders() const
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

const std::string &Request::getBody() const
{
    return mBody;
}

eConnectionStatus Request::getConnectionStatus() const
{
    return mConnectionStatus;
}