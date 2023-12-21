#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <vector>

class ServerManager
{
  private:
    std::vector<Server> mServers;

  public:
    ServerManager();
}
#endif