#include "HttpStatusInfos.hpp"

HttpStatusInfos::HttpStatusInfos()
{
}

void HttpStatusInfos::initHttpStatusInfos(char **envp)
{
    initHttpStatusReasons();
    initHttpErrorPages();
    setWebservRoot(envp);
}

void HttpStatusInfos::initHttpStatusReasons()
{
    mHttpStatusReasons[200] = "OK";
    mHttpStatusReasons[201] = "Created";
    mHttpStatusReasons[301] = "Moved Permanently";
    mHttpStatusReasons[400] = "Bad Request";
    mHttpStatusReasons[404] = "Not Found";
    mHttpStatusReasons[405] = "Method Not Allowed";
    mHttpStatusReasons[501] = "Not Implemented";
    mHttpStatusReasons[503] = "Service Unavailable";
}

void HttpStatusInfos::initHttpErrorPages()
{
    mHttpErrorPages[301] = "<html>" CRLF "<head><title>301 Moved Permanently</title></head>" CRLF "<body>" CRLF
                           "<center><h1>301 Moved Permanently</h1></center>" CRLF;
    mHttpErrorPages[400] = "<html>" CRLF "<head><title>400 Bad Request</title></head>" CRLF "<body>" CRLF
                           "<center><h1>400 Bad Request</h1></center>" CRLF;
    mHttpErrorPages[404] = "<html>" CRLF "<head><title>404 Not Found</title></head>" CRLF "<body>" CRLF
                           "<center><h1>404 Not Found</h1></center>" CRLF;
    mHttpErrorPages[405] = "<html>" CRLF "<head><title>405 Not Allowed</title></head>" CRLF "<body>" CRLF
                           "<center><h1>405 Not Allowed</h1></center>" CRLF;
    mHttpErrorPages[501] = "<html>" CRLF "<head><title>405 Not Allowed</title></head>" CRLF "<body>" CRLF
                           "<center><h1>405 Not Allowed</h1></center>" CRLF;
    mHttpErrorPages[503] = "<html>" CRLF "<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
                           "<body>" CRLF "<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF;
}

void HttpStatusInfos::setWebservRoot(char **envp)
{
    int i;
    for (i = 0; envp[i] != NULL; ++i)
    {
        std::string envStr = envp[i];
        if (envStr.find("WEBSERV_ROOT=") == 0)
        {
            mWebservRoot = envStr.substr(4);
            break;
        }
    }
    if (envp[i] == NULL)
    {
        throw std::runtime_error("webserv는 환경변수 WEBSERV_ROOT가 필요합니다.");
    }
}

const std::string &HttpStatusInfos::getHttpReason(const int statusCode)
{
    std::map<int, std::string>::const_iterator it;

    it = mHttpStatusReasons.find(statusCode);
    assert(it != mHttpStatusReasons.end());
    return it->second;
}

const std::string &HttpStatusInfos::getHttpErrorPage(const int statusCode)
{
    std::map<int, std::string>::const_iterator it;

    it = mHttpErrorPages.find(statusCode);
    assert(it != mHttpStatusReasons.end());
    return it->second;
}

const std::string &HttpStatusInfos::getWebservRoot()
{
    return mWebservRoot;
}