#ifndef SERVERINFOSTR_HPP
#define SERVERINFOSTR_HPP

#include <string>
#include <vector>

class ServerInfoStr
{
  private:
    std::string mServerStr;
    std::vector<std::string> mLocationStrs;

  public:
    ServerInfoStr(const std::string &serverStr, const std::vector<std::string> &locationStrs);
    const std::string &getServerStr() const;
    const std::vector<std::string> &getLocationStrs() const;
};

#endif