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
    // clang-format off
    Config(std::vector<ServerInfo> serverInfos);
		static std::vector<ServerInfo> mServerInfos;
    // Config(std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups);
		// static std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups;
    // clang-format on
    static Config *mInstance;

  public:
    static void createInstance(const std::string &httpString, const std::vector<ServerInfoStr> &serverInfoStrs);
    static Config &getInstance();
    static void deleteInstance(); // 새로운 config를 만들고 싶을때?
    static const std::vector<ServerInfo> &getServerInfos();
};
#endif
