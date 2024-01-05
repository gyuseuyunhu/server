#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "BlockBuilder.hpp"
#include "HttpBlock.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"
#include "ServerInfo.hpp"
#include "ServerInfoStr.hpp"
#include <cassert>
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
    static void deleteInstance(); // 새로운 config를 만들고 싶을때?
    static const std::vector<ServerInfo> &getServerInfos();
};
#endif
