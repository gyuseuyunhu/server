#include "BlockBuilder.hpp"
#include "Config.hpp"
#include "HttpStatusInfos.hpp"
#include "LocationBlock.hpp"
#include "Parse.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

int main(int ac, char **argv, char **envp)
{
    if (ac > 2)
    {
        return 1;
    }
    try
    {
        Parse a(ac == 2 ? argv[1] : "www/a.conf");
        Config::createInstance(a.getHttpStr(), a.getServerInfoStrs());
        HttpStatusInfos::initHttpStatusInfos(envp);
        ServerManager sm(Config::getServerInfos());
        sm.run();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}