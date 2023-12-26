#ifndef BLOCKBUILDER_HPP
#define BLOCKBUILDER_HPP

#include "HttpBlock.hpp"
#include "HttpDirective.hpp"
#include "LocationBlock.hpp"
#include "LocationDirective.hpp"
#include "ServerBlock.hpp"
#include "ServerDirective.hpp"
#include <string>
#include <vector>

enum unit
{
    killo = 1000,
    mega = 1000000,
    giga = 100000000,
};
enum blockType
{
    Http,
    Server,
    Location,
};
class BlockBuilder
{

  private:
    std::string mRoot;
    std::vector<std::string> mIndexs;
    // todo : errorpage를 어떻게 받을것인가
    // 예시 :  500 502 503 504 /50x.html; 또는 error_page  404 /404.html;
    std::vector<std::string> mErrorPages;
    unsigned int mClientMaxBodySize;

    unsigned int mPort;
    std::string mServerName;
    unsigned int mRedirectionCode;
    std::string mRedirectionPath;

    std::string mLocationPath;
    bool mIsAutoIndex;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;

    bool mIsMethodAllowed;
    HttpDirective mHttpDirective;
    ServerDirective mServerDirective;
    LocationDirective mLocationDirective;
    void updateConfig(const std::string &key, const std::string &value, bool isFirstValue, bool &isFirstIndex,
                      bool &isFirstErrorPage);
    std::string reduceMultipleSpaces(std::string token);
    unsigned int convertNumber(const std::string &valueString, bool hasUnit);

  public:
    BlockBuilder();
    void parseConfig(const enum blockType, const std::string &configStr);
    HttpBlock buildHttpBlock() const;
    ServerBlock buildServerBlock() const;
    LocationBlock buildLocationBlock() const;
    void resetServerBlockConfig(const HttpBlock &httpBlock);
    void resetLocationBlockConfig(const ServerBlock &serverBLock);
};

#endif // BLOCKBUILDER_HPP