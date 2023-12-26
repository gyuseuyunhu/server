#include "Server.hpp"

Server::Server(const ServerInfo &serverInfo)
    : mSocket(socket(AF_INET, SOCK_STREAM, 0)), mPort(serverInfo.getServerBlock().getPort())
{
    mServerInfos.push_back(serverInfo);
    if (mSocket == -1)
    {
        throw std::runtime_error("Error Server::Server() socket() - 소켓 생성 실패");
    }
}

const unsigned int &Server::getPort() const
{
    return mPort;
}

void Server::addServerInfo(const ServerInfo &serverInfo)
{
    mServerInfos.push_back(serverInfo);
}

// Debugging TODO - 추후 삭제
std::ostream &operator<<(std::ostream &os, const Server &serverb)
{
    os << "[Server] mSocket : " << serverb.mSocket << std::endl;
    os << "[Server] mPort : " << serverb.mPort << std::endl;

    std::vector<ServerInfo>::const_iterator it = serverb.mServerInfos.begin();

    os << "[Server] mServerInfosDivPort : " << std::endl;
    for (; it != serverb.mServerInfos.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;
    return os;
}

const std::vector<ServerInfo> &Server::getServerInfos() const
{
    return mServerInfos;
}