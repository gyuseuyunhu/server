#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

class ServerBlock;

class Server
{
  private:
    const int mSocketFd;
    const unsigned int mPort;
    const std::vector<ServerBlock> mServerBlocks;

  public:
};

#endif