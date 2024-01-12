#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "ConnectionEnum.hpp"
#include "Server.hpp"
#include <cstdlib>
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
        CONTENTS,
        CHUNKED,
        FINISH,
    };
    typedef std::map<std::string, std::string, CaseInsensitiveCompare> HeaderMap;

    const Server &mServer;
    std::string mMethod;
    std::string mPath;
    std::string mVersion;

    HeaderMap mHeaders;
    std::string mHost;
    std::string mBody;
    size_t mContentLength;

    eRequestLine mRequestLine;
    int mStatus;
    eConnectionStatus mConnectionStatus;

    unsigned long mChunkedSize;
    unsigned int mClientMaxBodySize;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;

    std::string mRedirectPath;

    void checkMethod(std::stringstream &ss);
    void checkPath(std::stringstream &ss);
    void checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer);

    void storeHeaderLine(const std::string &line);
    void storeHeaderMap(std::string buffer);

    void parseRequestHeader(std::string &buffer);
    void parseRequestContent(std::string &buffer);
    bool checkChunkedData(void);
    void parseChunkedContent(std::string &buffer);

  public:
    typedef HeaderMap::const_iterator MapIt;
    Request(const Server &server);
    ~Request();
    void parse(std::string &buffer);

    int getStatus() const;
    const std::map<std::string, std::string, CaseInsensitiveCompare> &getHeaders() const;
    const std::string &getHost() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    eConnectionStatus getConnectionStatus() const;
    const std::string &getRedirectionPath() const;
};

#endif