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

enum eRequestLine
{
    E_START_LINE,
    E_REQUEST_HEADER,
    E_REQUEST_CONTENTS,
    E_CHUNKED_CONTENTS
};

struct CaseInsensitiveCompare
{
    bool operator()(const std::string &a, const std::string &b) const;
};

class Request
{
  private:
    const Server &mServer;
    eHttpMethod mMethod;
    std::string mPath;
    std::string mVersion;

    std::map<std::string, std::string, CaseInsensitiveCompare> mHeaders;
    std::string mHost;
    std::string mBody;
    size_t mContentLength;

    eRequestLine mRequestLine;
    int mStatus;
    eConnectionStatus mConnectionStatus;

    unsigned int mChunkedSize;
    bool mIsChunkedData;
    unsigned int mClientMaxBodySize;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;

    int checkMethod(std::stringstream &ss);
    int checkPath(std::stringstream &ss);
    int checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer);

    int storeHeaderLine(const std::string &line);
    int storeHeaderMap(std::string buffer);

    void parseRequestHeader(std::string &buffer);
    void parseRequestContent(std::string &buffer);
    bool checkChunkedData(void);
    void parseChunkedContent(std::string &buffer);

  public:
    Request(const Server &server);
    ~Request();
    void parse(std::string &buffer);

    int getStatus() const;
    const std::map<std::string, std::string, CaseInsensitiveCompare> &getHeaders() const;
    const std::string &getHost() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    eConnectionStatus getConnectionStatus() const;
};

#endif