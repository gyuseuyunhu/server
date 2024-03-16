#include "Server.hpp"
#include "AcceptEvent.hpp"
#include "Kqueue.hpp"
#include <fcntl.h>
#include <unistd.h>

Server::Server(const ServerInfo &serverInfo) : mSocket(0), mPort(serverInfo.getServerBlock().getPort())
{
    mServerInfos.push_back(serverInfo);
}

Server::~Server()
{
    if (mSocket != 0)
    {
        close(mSocket);
    }
}

void Server::listen()
{
    struct sockaddr_in serverAddr;
    mSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        throw std::runtime_error("Error Server::Server(): socket() 소켓 생성 실패");
    }
    int yes = 1;
    if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        throw std::runtime_error("Error Server::Server(): setsockopt() 실패");
    }

    fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(mPort);

    if (bind(mSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        throw std::runtime_error("Error Server::listen(): 포트 bind 실패");
    }

    if (::listen(mSocket, 1000) == -1)
    {
        throw std::runtime_error("Error Server::listen(): listen 실패");
    }
    Kqueue::addEvent(new AcceptEvent(*this), EVFILT_READ);
}

int Server::getSocket() const
{
    return mSocket;
}

unsigned int Server::getPort() const
{
    return mPort;
}

void Server::addServerInfo(const ServerInfo &serverInfo)
{
    mServerInfos.push_back(serverInfo);
}

const LocationBlock Server::getLocationBlockForRequest(const std::string &host, const std::string &path) const
{
    std::vector<ServerInfo>::const_iterator serverIt = mServerInfos.begin();
    ServerInfo targetServerInfo = *serverIt;
    if (path == "")
    {
        return LocationBlock(targetServerInfo.getServerBlock());
    }
    ++serverIt;
    for (; serverIt != mServerInfos.end(); ++serverIt)
    {
        if (host == serverIt->getServerBlock().getServerName())
        {
            targetServerInfo = *serverIt;
            break;
        }
    }

    std::vector<LocationBlock>::const_iterator locIt = targetServerInfo.getLocationBlocks().begin();
    for (; locIt != targetServerInfo.getLocationBlocks().end(); ++locIt)
    {
        const std::string &locPath = locIt->getLocationPath();
        if (path.find(locPath) == 0)
        {
            if (locPath.size() == 1 || locPath == path || path[locPath.size() - 1] == '/' ||
                path[locPath.size()] == '/')
            {
                return *locIt;
            }
        }
    }
    return LocationBlock(targetServerInfo.getServerBlock());
}

const std::vector<ServerInfo> &Server::getServerInfos() const
{
    return mServerInfos;
}