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
        assert(false);
    }

    fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(mPort);

    // 서버 소켓에 주소 바인딩
    if (bind(mSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        throw std::runtime_error("Error Server::listen(): 포트 bind 실패");
    }
    // 클라이언트 연결 대기
    if (::listen(mSocket, 1000) == -1)
    {
        throw std::runtime_error("Error Server::listen(): listen 실패");
    }

    struct kevent newEvent;
    EV_SET(&newEvent, mSocket, EVFILT_READ, EV_ADD, 0, 0, new AcceptEvent(*this));
    Kqueue::addEvent(newEvent);
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

    // location은 길이가 긴 것부터 짧은 순으로 정렬이 되어 있는 상태
    for (; locIt != targetServerInfo.getLocationBlocks().end(); ++locIt)
    {
        if (path.find(locIt->getLocationPath()) == 0)
        {
            return *locIt;
        }
    }
    return LocationBlock(targetServerInfo.getServerBlock());
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