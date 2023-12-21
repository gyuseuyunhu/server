#include "Config.hpp"

Config *Config::mInstance = NULL;                            // Definition of mInstance
std::vector<ServerWithLocations> Config::mServerBlockGroups; // Definition of ServerBlockGroups

// clang-format off
Config::Config(std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > serverBlockGroups)
// clang-format on
{
    mServerBlockGroups = serverBlockGroups;
}

// clang-format off
	void Config::createInstance(std::string httpString, std::vector<std::pair<std::string, std::vector<std::string> > > mServerStr)
// clang-format on
{
    BlockBuilder builder;
    builder.parseConfig(Http, httpString);

    for (size_t i = 0; i < mServerStr.size(); ++i)
    {
        builder.resetServerBlockConfig();
        const std::string &serverString = mServerStr[i].first;
        const std::vector<std::string> &locationStrings = mServerStr[i].second;

        builder.parseConfig(Server, serverString);
        ServerBlock serverBlock = builder.buildServerBlock();
        // 디버그용 print
        std::cout << "server : " << std::endl << serverBlock << std::endl;

        std::vector<LocationBlock> locationBlocks;
        for (size_t j = 0; j < locationStrings.size(); ++j)
        {
            builder.resetLocationBlockConfig();
            builder.parseConfig(Location, locationStrings[j]);
            LocationBlock locationBlock = builder.buildLocationBlock();
            // 디버그용 print
            std::cout << "location :" << std::endl << locationBlock << std::endl;
            locationBlocks.push_back(locationBlock);
        }
        mServerBlockGroups.push_back(std::make_pair(serverBlock, locationBlocks));
    }
    mInstance = new Config(mServerBlockGroups);
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

const std::vector<ServerWithLocations> &Config::getServerBlockGroups()
{
    return mServerBlockGroups;
}
