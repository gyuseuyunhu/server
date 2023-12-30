#include "BlockBuilder.hpp"
#include "Config.hpp"
#include "LocationBlock.hpp"
#include "Parse.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

int main()
{
    try
    {
        Parse a("www/a.conf");
        std::cout << "serverStr : " << a.getHttpStr() << std::endl;
        Config::createInstance(a.getHttpStr(), a.getServerInfoStrs());
        ServerManager sm(Config::getInstance().getServerInfos());
        sm.run();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}