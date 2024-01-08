#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "ConnectionEnum.hpp"
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
    E_REQUEST_CONTENTS
};

struct CaseInsensitiveCompare
{
    bool operator()(const std::string &a, const std::string &b) const;
};

class Request
{
  private:
    eHttpMethod mMethod;
    std::string mPath;
    std::string mVersion;
    std::map<std::string, std::string, CaseInsensitiveCompare> mHeaders;
    std::string mHost;
    std::string mContent; // 자료형 좀 더 고민
    std::string mBody;
    unsigned int mContentLength;

    eRequestLine mRequestLine;
    int mStatus;
    eConnectionStatus mConnectionStatus;

    int checkMethod(std::stringstream &ss);
    int checkPath(std::stringstream &ss);
    int checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer);

    int storeHeaderLine(const std::string &line);
    int storeHeaderMap(std::string buffer);

    void parseRequestHeader(std::string &buffer);
    void parseRequestContent(std::string &buffer);

  public:
    Request();
    ~Request();
    void parse(std::string &buffer);

    int getStatus() const;
    const std::map<std::string, std::string, CaseInsensitiveCompare> &getHeaders() const;
    const std::string &getHost() const;
    const std::string &getPath() const;
    const std::string &getBody() const;
    eConnectionStatus getConnectionStatus() const;
    void clear();
};

#endif