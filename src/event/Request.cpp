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

Request::Request()
    : mMethod(""), mPath(""), mVersion(""), mHost(""), mBody(""), mContentLength(0), mRequestLine(START_LINE),
      mStatus(0), mConnectionStatus(KEEP_ALIVE), mIsChunked(false)
{
}

Request::~Request()
{
}

void Request::checkMethod(std::stringstream &ss)
{
    ss >> mMethod;
    if (mMethod.empty())
    {
        mStatus = BAD_REQUEST;
    }
}

void Request::checkPath(std::stringstream &ss)
{
    ss >> mPath;
    if (mPath.empty())
    {
        mStatus = BAD_REQUEST;
        return;
    }
    if (mPath[0] != '/')
    {
        mPath = "/" + mPath;
    }
}

void Request::checkHttpVersion(std::stringstream &ss)
{
    ss >> mVersion;
    if (mVersion != "HTTP/1.0" && mVersion != "HTTP/1.1")
    {
        mStatus = BAD_REQUEST;
    }
}

void Request::parseStartLine(std::string &buffer)
{
    size_t pos = buffer.find(CRLF);
    while (pos == 0)
    {
        buffer.erase(0, 2);
        pos = buffer.find(CRLF);
    }
    if (pos != std::string::npos)
    {
        std::stringstream ss(buffer.substr(0, pos));
        checkMethod(ss);
        checkPath(ss);
        checkHttpVersion(ss);
        buffer.erase(0, pos + 2);
        mRequestLine = HEADER;
    }
}

void Request::storeHeaderLine(const std::string &line)
{
    std::string headerKey;
    std::string headerVal;
    size_t pos = line.find(':');

    headerKey = line.substr(0, pos);
    if (mHeaders.find(headerKey) != mHeaders.end())
    {
        mStatus = BAD_REQUEST;
        return;
    }
    headerVal = trim(line.substr(pos + 1));
    mHeaders[headerKey] = headerVal;
}

void Request::storeHeaderMap(std::string buffer)
{
    size_t pos = 0;
    while ((pos = buffer.find(CRLF)) != std::string::npos)
    {
        storeHeaderLine(buffer.substr(0, pos));
        buffer.erase(0, pos + 2);
    }
}

void Request::parseRequestHeader(std::string &buffer)
{
    size_t pos = 0;
    if ((pos = buffer.find(CRLF CRLF)) != std::string::npos)
    {
        storeHeaderMap(buffer.substr(0, pos + 2));
        mHost = mHeaders["Host"];
        if (mHost.empty())
        {
            mStatus = BAD_REQUEST;
        }

        MapIt it = mHeaders.find("Connection");
        if (it != mHeaders.end() && it->second == "close")
        {
            mConnectionStatus = CONNECTION_CLOSE;
        }

        if (checkChunkedData())
        {
            buffer.erase(0, pos + 4);
            mRequestLine = CHUNKED;
            return;
        }

        it = mHeaders.find("Content-Length");
        if (it != mHeaders.end())
        {
            std::stringstream ss;
            ss << it->second;
            ss >> mContentLength;
            buffer.erase(0, pos + 4);
            mRequestLine = CONTENTS;
            return;
        }

        mRequestLine = FINISH;
        mBody = buffer.erase(0, pos + 4);
    }
}

bool Request::checkChunkedData(void)
{
    std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator iter =
        mHeaders.find("Transfer-Encoding");
    if (iter != mHeaders.end() && iter->second == "chunked")
    {
        mIsChunked = true;
        return true;
    }
    return false;
}

void Request::storeChunkedBody(std::string &buffer)
{
    size_t pos = buffer.find("0" CRLF CRLF);

    if (pos != std::string::npos)
    {
        mChunkedBody = buffer;
        mRequestLine = FINISH;
        return;
    }
}

int Request::parseChunkedBody(size_t clientMaxBodySize)
{
    while (1)
    {
        if (mChunkedSize == NO_SIZE)
        {
            size_t pos = mChunkedBody.find(CRLF);
            assert(pos != std::string::npos);
            std::string value = mChunkedBody.substr(0, pos);
            char *endptr;
            mChunkedSize = strtol(value.c_str(), &endptr, 16);
            if (endptr[0] != '\0')
            {
                return BAD_REQUEST;
            }
            if (mChunkedSize == NO_SIZE)
            {
                return OK;
            }
            mChunkedBody.erase(0, pos + 2);
        }
        else if (mChunkedSize != NO_SIZE /* && (mChunkedSize + 2) <= buffer.size()*/)
        {
            if (mChunkedBody.substr(mChunkedSize, 2) != CRLF)
            {
                return BAD_REQUEST;
            }
            mBody += mChunkedBody.substr(0, mChunkedSize);
            if (mBody.size() > clientMaxBodySize)
            {
                return CONTENT_TOO_LARGE;
            }
            mChunkedBody = mChunkedBody.substr(mChunkedSize + 2);
            mChunkedSize = NO_SIZE;
        }
        // else if ((mChunkedSize + 2) > buffer.size() || buffer.size() == 0 ||
        //          (mChunkedSize == 0 && (pos = buffer.find(CRLF)) == std::string::npos))
        // {
        //     return;
        // }
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
    else if (mBody.size() > mContentLength)
    {
        mStatus = 400;
    }
}

bool Request::tryParse(std::string &buffer)
{
    if (mRequestLine == START_LINE)
    {
        parseStartLine(buffer);
    }
    if (mRequestLine == HEADER)
    {
        parseRequestHeader(buffer);
    }
    if (mRequestLine == CONTENTS)
    {
        parseRequestContent(buffer);
    }
    if (mRequestLine == CHUNKED)
    {
        storeChunkedBody(buffer);
    }
    if (mRequestLine == FINISH)
    {
        return true;
    }
    return false;
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
const std::string &Request::getMethod() const
{
    return mMethod;
}

bool Request::isChunked() const
{
    return mIsChunked;
}

eConnectionStatus Request::getConnectionStatus() const
{
    return mConnectionStatus;
}
