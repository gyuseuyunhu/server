#include "Server.hpp"
#include "Accept.hpp"
#include "Kqueue.hpp"
#include <fcntl.h>
#include <unistd.h>

Server::Server(const ServerInfo &serverInfo)
    : mSocket(socket(AF_INET, SOCK_STREAM, 0)), mPort(serverInfo.getServerBlock().getPort())
{
    fcntl(mSocket, F_SETFL, O_NONBLOCK);
    mServerInfos.push_back(serverInfo);
    if (mSocket == -1)
    {
        throw std::runtime_error("Error Server::Server(): socket() 소켓 생성 실패");
    }
}

Server::~Server()
{
    close(mSocket);
}

void Server::listen()
{
    struct sockaddr_in serverAddr;

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
    if (::listen(mSocket, 5) == -1)
    {
        throw std::runtime_error("Error Server::listen(): listen 실패");
    }

    struct kevent newEvent;
    EV_SET(&newEvent, mSocket, EVFILT_READ, EV_ADD, 0, 0, new Accept(*this));
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

// 이걸 받는 response에서 해줘야 할것
// 1. 반환값 iterator를 돌면서 open할 수 있는지 확인
// 	  - 할 수 있으면 읽고 response 객체에 저장
// 2. 다 돌았는데 open 할 수 있는게 없다!
//    - autoindex, 301이 있음
//    - 파일이면 같은이름의 폴더가 있는지 확인(opendir)
//    - 있으면 301 없으면 404
//    - 폴더이면 autoindex가 있는지 확인
//    - on이면 autoindex, off 이면 404
const std::vector<std::string> Server::getFilePath(const std::string &host, const std::string &path,
                                                   bool &isFolder) const
{
    std::vector<ServerInfo>::const_iterator serverIt = mServerInfos.begin();
    std::vector<std::string> indexsPath;
    std::vector<std::string> filePath;
    std::string root;
    ServerInfo targetServerInfo = *serverIt;
    ++serverIt;
    for (; serverIt != mServerInfos.end(); ++serverIt)
    {
        if (host == serverIt->getServerBlock().getServerName())
        {
            targetServerInfo = *serverIt;
        }
    }

    std::vector<LocationBlock>::const_iterator locIt = targetServerInfo.getLocationBlocks().begin();
    // 지금 location은 길이순으로 정렬이 되어 있는 상태
    // location /abc/def/ {};, location /abc/의 경우
    // get /abc/를 요청하면
    // 두 번 find로 찾지만 결국엔 locaiton /abc/를 찾는다
    for (; locIt != targetServerInfo.getLocationBlocks().end(); ++locIt)
    {
        if (locIt->getLocationPath().find(path) != std::string::npos)
        {
            indexsPath = locIt->getIndexs();
            root = locIt->getRoot();
        }
    }

    // 일치하지 않는 경우
    // 1. location / 가 있는지 찾는다
    if (indexsPath.size() == 0)
    {
        --locIt;
        if (locIt->getLocationPath() == "/")
        {
            indexsPath = locIt->getIndexs();
            root = locIt->getRoot();
        }
    }
    // 2. 아니면 서버 블록을 따라 간다.
    else
    {
        indexsPath = targetServerInfo.getServerBlock().getIndexs();
        root = targetServerInfo.getServerBlock().getRoot();
    }

    // 폴더를 요청한 경우
    // index위치 벡터를 보내줌
    if (path[path.size() - 1] == '/')
    {
        isFolder = true;
        std::vector<std::string>::iterator indexIt = indexsPath.begin();
        for (; indexIt != indexsPath.end(); ++indexIt)
        {
            *indexIt = root + *indexIt;
        }
        return indexsPath;
    }
    // 파일을 요청한 경우
    // 파일위치를 보내줌
    else
    {
        isFolder = false;
        size_t pos = path.find_last_of('/');
        if (pos == std::string::npos)
        {
            // 지금 논리상 여기 오면 안됨
            assert(false);
        }
        filePath.push_back(root + path.substr(pos));
        return filePath;
    }
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