#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include "LocationBlock.hpp"
#include "ServerBlock.hpp"

class ServerInfo
{
  private:
    ServerBlock mServerBlock;
    std::vector<LocationBlock> mLocationBlocks;

  public:
    ServerInfo(const ServerBlock &serverBlock, const std::vector<LocationBlock> &locationBlocks);
    const ServerBlock &getServerBlock() const;
    const std::vector<LocationBlock> &getLocationBlocks() const;

    friend std::ostream &operator<<(std::ostream &os, const ServerInfo &serverInfo);
};
#endif