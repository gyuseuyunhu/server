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
