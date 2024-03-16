#ifndef BLOCKBUILDER_HPP
#define BLOCKBUILDER_HPP

#include "HttpBlock.hpp"
#include "HttpDirective.hpp"
#include "LocationBlock.hpp"
#include "LocationDirective.hpp"
#include "ServerBlock.hpp"
#include "ServerDirective.hpp"
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

enum blockType
{
    E_HTTP,
    E_SERVER,
    E_LOCATION,
};

class BlockBuilder
{

  private:
    enum unit
    {
        E_KILLO = 1024,
        E_MEGA = 1024 * 1024,
        E_GIGA = 1024 * 1024 * 1024,
        E_CLIENT_MAX_BODY_SIZE = 2147483647,
    };
    enum defalutValue
    {
        E_DEFAULT_CLIENT_BODY_SIZE = 1024 * 1024,
        E_DEFAULT_LISTEN_PORT = 80,
    };
    static const std::string DEFAULT_ROOT;
    static const std::string DEFAULT_SERVER_NAME;

    std::string mRoot;
    std::vector<std::string> mIndexs;
    std::vector<unsigned int> mErrorCodes;
    std::map<int, std::string> mErrorPages;
    unsigned int mClientMaxBodySize;

    unsigned int mPort;
    std::string mServerName;
    std::string mRedirectionPath;

    std::string mLocationPath;
    bool mIsAutoIndex;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;
    bool mIsMethodAllowed;
    std::string mCgiPath;
    std::string mCgiExtension;
    std::string mCgiUploadDir;

    HttpDirective mHttpDirective;
    ServerDirective mServerDirective;
    LocationDirective mLocationDirective;
    void updateConfig(const std::string &key, const std::string &value, bool isFirstValue, bool &isFirstIndex,
                      bool &isFirstErrorPage);
    std::string reduceMultipleSpaces(std::string token);
    bool tryConvertNumber(const std::string &valueString, bool hasUnit, unsigned int &result);

    // handle function
    void handleRoot(const std::string &value);
    void handleIndex(const std::string &value, bool &isFirstIndex);
    void handleErrorPage(const std::string &value, bool &isMadeErrorPages);
    void handleClientMaxBodySize(const std::string &value);
    void handleListen(const std::string &value);
    void handleAutoindex(const std::string &value);
    void handleLimitExcept(const std::string &value, bool isFirstValue, bool &isMethodAllowed);

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