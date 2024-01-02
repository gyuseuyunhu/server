#include "ServerInfoStr.hpp"

ServerInfoStr::ServerInfoStr(const std::string &serverStr, const std::vector<std::string> &locationStrs)
    : mServerStr(serverStr), mLocationStrs(locationStrs)
{
}

const std::string &ServerInfoStr::getServerStr() const
{
    return mServerStr;
}

const std::vector<std::string> &ServerInfoStr::getLocationStrs() const
{
    return mLocationStrs;
}