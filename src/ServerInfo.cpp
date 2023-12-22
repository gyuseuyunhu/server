#include "ServerInfo.hpp"

ServerInfo::ServerInfo(const ServerBlock &serverBlock, const std::vector<LocationBlock> &locationBlocks)
    : mServerBlock(serverBlock), mLocationBlocks(locationBlocks)
{
}

const ServerBlock &ServerInfo::getServerBlock() const
{
    return mServerBlock;
}

const std::vector<LocationBlock> &ServerInfo::getLocationBlocks() const
{
    return mLocationBlocks;
}

std::ostream &operator<<(std::ostream &os, const ServerInfo &serverInfo)
{
    // locationBlock.mLocationPath
    std::vector<LocationBlock>::const_iterator it = serverInfo.mLocationBlocks.begin();
    os << "print ServerInfo " << std::endl;
    os << "Server " << std::endl;
    os << serverInfo.mServerBlock << std::endl;
    os << "Location " << std::endl;
    for (; it != serverInfo.mLocationBlocks.end(); ++it)
    {
        os << *it << std::endl;
    }
    os << std::endl;
    return os;
}