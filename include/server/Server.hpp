#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

#include "ServerInfo.hpp"

class Server
{
  private:
    int mSocket;
    const unsigned int mPort;
    std::vector<ServerInfo> mServerInfos;

  public:
    Server(const ServerInfo &serverInfo);
    ~Server();
    int getSocket() const;
    unsigned int getPort() const;
    const std::vector<ServerInfo> &getServerInfos() const;
    void addServerInfo(const ServerInfo &serverInfo);
    void listen();
    const LocationBlock getLocationBlockForRequest(const std::string &host, const std::string &path) const;
};

#endif