#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "BlockBuilder.hpp"
#include "HttpBlock.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

typedef std::vector<std::string> LocationVec;
typedef std::pair<std::string, LocationVec> ServerLocPair;
typedef std::vector<LocationBlock> LocationList;
typedef std::pair<ServerBlock, LocationList> ServerWithLocations;

class Config
{
  private:
    Config();
    Config &operator=(const Config &rhs);
    // clang-format off
    Config(std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups);
		static std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups;
    // clang-format on

    static Config *mInstance;

  public:
    // clang-format off
    static void createInstance(std::string httpString, std::vector<std::pair<std::string, std::vector<std::string> > > mServerStr);
    // clang-format on
    static Config &getInstance();
    static void deleteInstance(); // 새로운 config를 만들고 싶을때?

    static const std::vector<ServerWithLocations> &getServerBlockGroups();
};
#endif
