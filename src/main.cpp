#include "BlockBuilder.hpp"
#include "Config.hpp"
#include "HttpStatusInfos.hpp"
#include "LocationBlock.hpp"
#include "Parse.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include <csignal>

int main(int ac, char **argv, char **envp)
{
    if (ac > 2)
    {
        return 1;
    }
    try
    {
        signal(SIGCHLD, SIG_IGN);
        Parse parse(ac == 2 ? argv[1] : "www/default.conf");
        HttpStatusInfos::initHttpStatusInfos(envp);
        Config::createInstance(parse.getHttpStr(), parse.getServerInfoStrs());
        ServerManager sm(Config::getServerInfos());
        Config::deleteInstance();
        sm.run();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}