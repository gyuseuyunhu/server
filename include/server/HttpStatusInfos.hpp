#ifndef HTTPSTATUSINFOS_HPP
#define HTTPSTATUSINFOS_HPP

#include <map>
#include <string>

class HttpStatusInfos
{
  private:
    static std::map<int, std::string> mHttpStatusReason;
};

#endif