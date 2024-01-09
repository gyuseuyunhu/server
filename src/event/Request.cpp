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
    : mMethod(E_GET), mPath(""), mVersion("HTTP/1.1"), mHost(""), mBody(""), mContentLength(0),
      mRequestLine(E_START_LINE), mStatus(0), mConnectionStatus(KEEP_ALIVE), mIsChunkedData(false)
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

        if (mHeaders.find("Connection") != mHeaders.end())
        {
            if (mHeaders["Connection"] == "close")
            {
                mConnectionStatus = CONNECTION_CLOSE;
            }
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

/*
    23.01.09 추가
*/
bool Request::checkChunkedData(void)
{
    // if (mHeaders.find("Content-Length") == mHeaders.end())
    //  TODO - Content-Length 지시어가 없을 때 에러처리 하는 부분을 찾아 수정 필요
    //  (Content-Length가 없어도 Transfer-Enconding이 chunked이면 에러 아님)
    //  Content-Length가 있어도 chunked 데이터 일 수 있음
    std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator iter =
        mHeaders.find("Transfer-Encoding");
    if (iter != mHeaders.end())
    {
        if (iter->second == "chunked")
        {
            std::cout << "ture!!!! " << std::endl;
            return true;
        }
    }
    return false;
}

/*
    23.01.09 추가
*/
void Request::parseChunkedContent(std::string &buffer)
{
    // size, data 순서는 반드시 지켜진다는 전제(size, size 또는 data, data 순으로는 못 옴)
    //  chucked Size 16진법 CRLF
    //  본문에는 붙이지 않고, 들어오는 message 사이즈가 size랑 일치하는지 확인
    //   - 일치하지 않고 CRLF가 오면 에러
    //   - 에러코드 변경(400?)

    // 3
    // 1 (3) ss << 1, 13
    // 3CRLF
    // 3CRLFabcCRLF
    // 3CRLFa
    // bcCRLF
    // bc
    std::stringstream ss;
    size_t pos = 0;
    // if ((pos = buffer.find(CRLF CRLF)) != std::string::npos)
    // {
    //     if (!mFirstCRLF)
    //     {
    //         buffer = buffer.substr(pos + 4);
    //         return;
    //     }
    //     else
    //     {
    //         mStatus = 400;
    //         return;
    //     }
    // }
    std::cout << buffer << std::endl;
    if ((pos = buffer.find(CRLF)) != std::string::npos)
    {
        if (mIsChunkedData) // 지금 버퍼가 내용이다
        {
            std::cout << "여보세요" << std::endl;
            mChunkedData = buffer.substr(0, pos);    // 바디 붙이기
            if (mChunkedSize != mChunkedData.size()) // 사이즈와 내용이 다르다
            {
                mStatus = 400;
                return; // 40x 에러 리턴
            }
            mBody += mChunkedData; // 정상적인 경우 본문에 계속 더해서 내용 붙이기
            mIsChunkedData = false;
        }
        else
        {
            std::string value = buffer.substr(0, pos);
            char *endptr;
            ss << std::strtol(value.c_str(), &endptr, 16);
            ss >> mChunkedSize;
            if (endptr[0] != '\0')
            {
                std::cout << mBody << std::endl;
                mStatus = 400;
                return;
            }
            if (mChunkedSize == 0)
            {
                if (mBody.size() == 0 || mBody.size() > mClientMaxBodySize)
                {
                    mStatus = 400;
                    return;
                }
                std::cout << mBody << std::endl;
                mStatus = 200;
                return;
            }
            mIsChunkedData = true;
        }
        buffer = buffer.substr(pos + 2);
    }
    // chunked message 문자열 CRLF
    // 마지막 chunked size = 0일 때까지 읽기
    // chunked data가 중간에 read()에서 짤릴 수 있어서 CRLF까지 계속 덧붙이기

    // 보완
    //  - 처음 rnrn이 아니고 이상한 문자 들어오면 400, 현재는 rnrn계속 기다림.

    // overflow ?? bodymaxsize를 대조
    //
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
    if (mStatus == 400 || mStatus == 501)
    {
        mConnectionStatus = CONNECTION_CLOSE;
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
    mBody = "";
    mStatus = 0;
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