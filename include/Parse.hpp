#ifndef PARSE_HPP
#define PARSE_HPP

#include "ServerInfoStr.hpp"
#include <fstream>
#include <iostream>

class Parse
{
  private:
    std::ifstream mFile;
    int mDepth;
    std::string mHttpStr;
    std::vector<ServerInfoStr> mServerInfoStrs;
    void storeHttpStr(std::string &line);
    void storeServerStr(std::string &line);
    std::string storeLocationStr(std::string &line);

    void skipBeforeHttp(std::string &line);
    void skipDirective(std::string &line, int num);
    void checkOpenBrace(std::string &line);
    void checkCloseBrace(std::string &line);
    void checkLocationPath(std::string &line, std::string &locationStr);
    void parseElseLine(std::string &str, std::string &line);

  public:
    Parse(const char *path);
    ~Parse();
    const std::string &getHttpStr() const;
    const std::vector<ServerInfoStr> &getServerInfoStrs() const;
};

#endif