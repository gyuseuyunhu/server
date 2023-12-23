#include "Config.hpp"

Config *Config::mInstance = NULL;             // Definition of mInstance
std::vector<ServerInfo> Config::mServerInfos; // Definition of ServerBlockGroups

Config::Config(std::vector<ServerInfo> serverInfos)
{
    mServerInfos = serverInfos;
    // 디버그용 프린트
    std::vector<ServerInfo>::const_iterator it = mServerInfos.begin();
    for (; it != mServerInfos.end(); ++it)
    {
        std::cout << "*------------*" << std::endl;
        std::cout << *it << std::endl;
        std::cout << "*------------*" << std::endl;
    }
}

// clang-format off
void Config::createInstance(const std::string &httpString, const std::vector<ServerInfoStr> &serverInfoStrs)
// clang-format on
{
    BlockBuilder builder;
    builder.parseConfig(Http, httpString);
    const HttpBlock httpblock = builder.buildHttpBlock();

    for (size_t i = 0; i < serverInfoStrs.size(); ++i)
    {
        builder.resetServerBlockConfig(httpblock);
        const std::string &serverString = serverInfoStrs[i].getServerStr();
        const std::vector<std::string> &locationStrings = serverInfoStrs[i].getLocationStrs();

        builder.parseConfig(Server, serverString);
        ServerBlock serverBlock = builder.buildServerBlock();

        std::vector<LocationBlock> locationBlocks;
        for (size_t j = 0; j < locationStrings.size(); ++j)
        {
            builder.resetLocationBlockConfig(serverBlock);
            builder.parseConfig(Location, locationStrings[j]);
            LocationBlock locationBlock = builder.buildLocationBlock();
            locationBlocks.push_back(locationBlock);
        }
        mServerInfos.push_back(ServerInfo(serverBlock, locationBlocks));
    }
    mInstance = new Config(mServerInfos);
}

Config &Config::getInstance()
{
    if (mInstance == NULL)
    {
        throw std::runtime_error("Before calling getInstance, it is necessary to createInstance first.");
    }
    return *mInstance;
}

void Config::deleteInstance()
{
    if (mInstance == NULL)
    {
        throw std::runtime_error("Before calling deleteInstance, it is necessary to createInstance first.");
    }
    delete mInstance;
    mInstance = NULL;
}

const std::vector<ServerInfo> &Config::getServerInfos()
{
    return mServerInfos;
}
