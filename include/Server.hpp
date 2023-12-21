#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

#include "LocationBlock.hpp"
#include "ServerBlock.hpp"

class ServerBlock;

class Server
{
  private:
    typedef std::vector<LocationBlock> LocationBlocks;
    const int mSocket;
    const unsigned int mPort;
    std::vector<ServerBlock> mServerBlocks;
    std::vector<LocationBlocks> mLocationBlocks;

  public:
};

#endif