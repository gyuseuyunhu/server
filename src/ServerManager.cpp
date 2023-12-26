#include "ServerManager.hpp"
#include "Kqueue.hpp"

// 서버 매니저의 역할
// 서버를 생성
// kqueue 언제 listenSocket을 넣어줄지

// 서버 매니저 생성자에서 서버를 생성하는데 동일 포트면 소켓을 열지 않고
// 서버
// 에러 체크도 해줘 -> 같은 포트에 같은 서버이름이면 중복에러

// run에서 해줄지

// Kevent::handleEvents() 호출

ServerManager::ServerManager(const std::vector<ServerInfo> &serverInfos)
{
    for (size_t i = 0; i < serverInfos.size(); ++i)
    {
        pushServerInfo(serverInfos[i]);
    }
}

// 포트 번호 체크 및 결과값 반환
void ServerManager::pushServerInfo(const ServerInfo &serverInfo)
{
    unsigned int checkPort = serverInfo.getServerBlock().getPort();
    std::string checkServerName = serverInfo.getServerBlock().getServerName();

    for (size_t i = 0; i < mServers.size(); ++i)
    {
        if (checkPort == mServers[i].getPort())
        {
            const std::vector<ServerInfo> &checkServerInfos = mServers[i].getServerInfos();
            for (size_t j = 0; j < checkServerInfos.size(); ++j)
            {
                if (checkServerName == checkServerInfos[j].getServerBlock().getServerName())
                {
                    throw std::runtime_error("Error ServerManager::checkServerInfo(): 동일 포트 동일 호스트 중복 에러");
                }
                else
                {
                    mServers[i].addServerInfo(serverInfo);
                    return;
                }
            }
        }
    }
    mServers.push_back(Server(serverInfo));
}

void ServerManager::run()
{
    // Debugging TODO - 추후 삭제
    // std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    // std::vector<Server>::iterator iter = mServers.begin();
    // for (; iter != mServers.end(); ++iter)
    // {
    //     std::cout << *iter << std::endl;
    // }
    // std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    // Debugging

    // while (1)
    // {
    //     Kqueue::handleEvents();
    // }
}