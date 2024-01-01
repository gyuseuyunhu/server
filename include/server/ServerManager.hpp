#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <iostream>
#include <vector>

class ServerManager
{
  private:
    std::vector<Server> mServers;
  	void pushServerInfo(const ServerInfo &serverInfo);

  public:
    ServerManager(const std::vector<ServerInfo> &serverInfos);
    void run();
};

#endif