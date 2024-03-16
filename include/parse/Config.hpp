#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "BlockBuilder.hpp"
#include "HttpBlock.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"
#include "ServerInfo.hpp"
#include "ServerInfoStr.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

class Config
{
  private:
    Config();
    Config &operator=(const Config &rhs);
    Config(std::vector<ServerInfo> serverInfos);
    static std::vector<ServerInfo> mServerInfos;
    static Config *mInstance;
    static bool compareByPathLength(const LocationBlock &a, const LocationBlock &b);

  public:
    static void createInstance(const std::string &httpString, const std::vector<ServerInfoStr> &serverInfoStrs);
    static const Config &getInstance();
    static void deleteInstance();
    static const std::vector<ServerInfo> &getServerInfos();
};
#endif
