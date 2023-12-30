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
    else
    {
        throw 405; // Method NOT ALLOWED
    }
}

void Request::checkPath(std::stringstream &ss)
{
    ss >> mPath;
    if (mPath != "")
    {
        throw 400; // BAD_REQUEST
    }
}

void Request::checkHttpVersion(std::stringstream &ss)
{
    std::string version;
    ss >> version;
    if (version != "HTTP/1.1")
    {
        throw 400; // BAD_REQUEST
    }
}

void Request::parseStartLine(std::string &buffer, eRequestLine &requestLine)
{
    size_t pos;
    if ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::stringstream ss(buffer.substr(0, pos));
        checkMethod(ss);
        checkPath(ss);
        checkHttpVersion(ss);
        buffer = buffer.substr(pos + 2);
        requestLine = E_REQUEST_HEADER;
    }
}

void Request::storeHeaderMap(std::string buffer)
{
    std::string line;
    std::string headerKey;
    std::string headerVal;
    while (1)
    {
        size_t pos;
        if ((pos = buffer.find("\r\n") != std::string::npos))
        {
            line = buffer.substr(0, pos);
            pos = line.find(':', pos);
            if (pos == std::string::npos)
            {
                throw 400; // Bad Request
            }
            headerKey = trim(line.substr(0, pos));
            headerVal = trim(line.substr(pos + 1));
            if (headerVal.size() == 0)
            {
                throw 400;
            }
            mHeaders[headerKey] = headerVal;
        }
        else
        {
            break;
        }
    }
}

void Request::parseRequestHeader(std::string &buffer, eRequestLine &requestLine)
{
    size_t pos;
    if ((pos = buffer.find("\r\n\r\n")) != std::string::npos)
    {
        storeHeaderMap(buffer.substr(0, pos + 2));
        buffer = buffer.substr(pos + 4);
        requestLine = E_REQUEST_CONTENTS;
    }
}

void Request::parseRequestContent(std::string &buffer)
{
    mContent += buffer;
    buffer = "";
}

void Request::parse(std::string &buffer, eRequestLine &requestLine)
{
    if (requestLine == E_START_LINE)
    {
        parseStartLine(buffer, requestLine);
    }
    if (requestLine == E_REQUEST_HEADER)
    {
        parseRequestHeader(buffer, requestLine);
    }
    if (requestLine == E_REQUEST_CONTENTS)
    {
        parseRequestContent(buffer);
    }
}