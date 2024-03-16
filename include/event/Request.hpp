#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpEnum.hpp"
#include "Server.hpp"
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

enum eHttpMethod
{
    E_GET,
    E_POST,
    E_DELETE,
    E_NOT_IMPLEMENT,
    E_BAD_REQUEST
};

struct CaseInsensitiveCompare
{
    bool operator()(const std::string &a, const std::string &b) const;
};

class Request
{
  private:
    enum
    {
        NO_SIZE = 0
    };

    enum eRequestLine
    {
        START_LINE,
        HEADER,
        BODY,
        CHUNKED,
        FINISH,
    };
    typedef std::map<std::string, std::string, CaseInsensitiveCompare> HeaderMap;

    std::string mMethod;
    std::string mPath;
    std::string mVersion;

    HeaderMap mHeaders;
    std::string mHost;
    std::string mBody;
    std::string mChunkedBody;
    size_t mContentLength;

    eRequestLine mRequestLine;
    int mStatus;
    eConnectionStatus mConnectionStatus;
    bool mIsChunked;
    unsigned long mChunkedSize;

    std::string mPathInfo;
    void checkMethod(std::stringstream &ss);
    void checkPath(std::stringstream &ss);
    void checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer);

    void storeHeaderLine(const std::string &line);
    void storeHeaderMap(std::string buffer);

    void parseHeader(std::string &buffer);
    void parseBody(std::string &buffer);
    bool checkChunkedData(void);
    void parseChunkedBody(std::string &buffer);

  public:
    typedef HeaderMap::const_iterator MapIt;
    Request();
    ~Request();
    bool tryParse(std::string &buffer);

    char **getCgiEnvp(const LocationBlock &lb) const;
    void delCgiEnvp(char **cgiEnvp);

    int getStatus() const;
    const std::map<std::string, std::string, CaseInsensitiveCompare> &getHeaders() const;
    const std::string &getHost() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    const std::string &getMethod() const;
    bool isChunked() const;
    void setPathInfo(const std::string &pathInfo);
    eConnectionStatus getConnectionStatus() const;
};

#endif