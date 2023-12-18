#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Directive.hpp"
#include "HttpBlock.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"
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
    static Config *mInstance;
    static Directive mDirective;
    static HttpBlock mHttpBlock;
    // clang-format off
		const std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups;
		Config(HttpBlock httpBlock, std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups);
    // clang-format on
    static HttpBlock createHttpBlock(std::string httpStr);

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
