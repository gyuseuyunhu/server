#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ADirective.hpp"
#include "HttpBlock.hpp"
#include "HttpDirective.hpp"
#include "LocationBlock.hpp"
#include "LocationDirective.hpp"
#include "ServerBlock.hpp"
#include "ServerDirective.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

enum unit
{
    killo = 1000,
    mega = 1000000,
    giga = 100000000,
};

class Config
{
  private:
    Config();
    Config &operator=(const Config &rhs);
    static Config *mInstance;
    static HttpBlock mHttpBlock;
    // clang-format off
		const std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups;
		Config(HttpBlock httpBlock, std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups);
    // clang-format on
    static HttpBlock createHttpBlock(std::string httpStr);
    static std::string reduceMultipleSpaces(std::string token);
    static unsigned int convertBodySize(std::string &valueString);

  public:
    // ~Config();
    // clang-format off
    static void createInstance(std::string httpString);
                                 //std::vector<std::pair<std::string, std::vector<std::string> > > mServerStr);
    // clang-format on
    static Config &getInstance();
    static void deleteInstance(); // 새로운 config를 만들고 싶을때?
};
#endif
