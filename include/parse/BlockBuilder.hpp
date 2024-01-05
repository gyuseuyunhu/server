#ifndef BLOCKBUILDER_HPP
#define BLOCKBUILDER_HPP

#include "HttpBlock.hpp"
#include "HttpDirective.hpp"
#include "LocationBlock.hpp"
#include "LocationDirective.hpp"
#include "ServerBlock.hpp"
#include "ServerDirective.hpp"
#include <map>
#include <string>
#include <vector>

enum unit
{
    E_KILLO = 1000,
    E_MEGA = 1000000,
    E_GIGA = 100000000,
};

enum blockType
{
    E_HTTP,
    E_SERVER,
    E_LOCATION,
};

class BlockBuilder
{

  private:
    std::string mRoot;
    std::vector<std::string> mIndexs;
    std::vector<unsigned int> mErrorCodes;
    std::map<int, std::string> mErrorPages;
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
    bool tryConvertNumber(const std::string &valueString, bool hasUnit, unsigned int &result);

  public:
    BlockBuilder();
    void parseConfig(const enum blockType, const std::string &configStr);
    HttpBlock buildHttpBlock() const;
    ServerBlock buildServerBlock() const;
    LocationBlock buildLocationBlock() const;
    void resetServerBlockConfig(const HttpBlock &httpBlock);
    void resetLocationBlockConfig(const ServerBlock &serverBLock);
};

#endif