#include "ServerManager.hpp"
#include "Kqueue.hpp"

ServerManager::ServerManager(const std::vector<ServerInfo> &serverInfos)
{
    for (size_t i = 0; i < serverInfos.size(); ++i)
    {
        pushServerInfo(serverInfos[i]);
    }
    Kqueue::init();
    for (size_t i = 0; i < mServers.size(); ++i)
    {
        mServers[i].listen();
    }
}

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
    while (1)
    {
        Kqueue::handleEvents();
    }
}