#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

#include "ServerInfo.hpp"

class Server
{
  private:
    const int mSocket;
    const unsigned int mPort;
    std::vector<ServerInfo> mServerInfos;

  public:
    Server(const ServerInfo &serverInfo);
    ~Server();
    const int getSocket() const;
    const unsigned int getPort() const;
    const std::vector<ServerInfo> &getServerInfos() const;
    void addServerInfo(const ServerInfo &serverInfo);
    void listen();
    const std::vector<std::string> getFilePath(const std::string &host, const std::string &path) const;

    // Debugging - TODO : 추후 삭제
    friend std::ostream &operator<<(std::ostream &os, const Server &serverb);
};

#endif