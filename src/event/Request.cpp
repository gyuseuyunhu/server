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
      mStatus(0), mConnectionStatus(KEEP_ALIVE), mIsChunked(false), mChunkedSize(0)
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
    if (headerKey == "X-Secret-Header-For-Test")
    {
        headerKey = "HTTP_" + headerKey;
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

void Request::parseHeader(std::string &buffer)
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
            mRequestLine = BODY;
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

void Request::parseChunkedBody(std::string &buffer)
{
    while (mChunkedSize + 2 <= buffer.size())
    {
        if (mChunkedSize == NO_SIZE)
        {
            size_t pos = buffer.find(CRLF);
            if (pos == std::string::npos)
            {
                return;
            }
            std::string value = buffer.substr(0, pos);
            char *endptr;
            mChunkedSize = strtol(value.c_str(), &endptr, 16);
            if (endptr[0] != '\0')
            {
                mStatus = BAD_REQUEST;
                return;
            }
            if (mChunkedSize == 0)
            {
                mRequestLine = FINISH;
                return;
            }
            buffer.erase(0, pos + 2);
        }
        else
        {
            if (buffer[mChunkedSize] != '\r' && buffer[mChunkedSize + 1] != '\n')
            {
                mStatus = BAD_REQUEST;
                return;
            }
            mBody += buffer.substr(0, mChunkedSize);
            buffer.erase(0, mChunkedSize + 2);
            mChunkedSize = NO_SIZE;
        }
    }
}

void Request::parseBody(std::string &buffer)
{
    mBody += buffer;
    buffer = "";
    if (mBody.size() == mContentLength)
    {
        mStatus = OK;
        mRequestLine = FINISH;
    }
    // ContentLength보다 더 많이 들어왔을 때
    else if (mBody.size() > mContentLength)
    {
        mStatus = BAD_REQUEST;
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
        parseHeader(buffer);
    }
    if (mRequestLine == BODY)
    {
        parseBody(buffer);
    }
    if (mRequestLine == CHUNKED)
    {
        parseChunkedBody(buffer);
    }
    if (mStatus == BAD_REQUEST)
    {
        mConnectionStatus = CONNECTION_CLOSE;
        return true;
    }
    return mRequestLine == FINISH;
}

char **Request::getCgiEnvp() const
{
    HttpStatusInfos::addEnv("REQUEST_METHOD=" + mMethod);
    HttpStatusInfos::addEnv("SERVER_PROTOCOL=" + mVersion);
    // todo pathInfo가 무엇인지 확인 필요
    HttpStatusInfos::addEnv("PATH_INFO=/asdf/asdf");
    MapIt it = mHeaders.begin();
    for (; it != mHeaders.end(); ++it)
    {
        size_t i;
        char env[it->first.size() + it->second.size() + 2];
        strcpy(env, it->first.c_str());
        for (i = 0; i < it->first.size(); ++i)
        {
            if (env[i] == '-')
            {
                env[i] = '_';
            }
            else
            {
                env[i] = std::toupper(env[i]);
            }
        }
        env[i] = '=';
        strcpy(env + i + 1, it->second.c_str());
        env[i + it->second.size() + 1] = '\0';
        HttpStatusInfos::addEnv(env);
    }
    return HttpStatusInfos::allocateNewEnvp();
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
