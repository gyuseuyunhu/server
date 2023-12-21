#include "BlockBuilder.hpp"
#include "Config.hpp"
#include "LocationBlock.hpp"
#include "Parse.hpp"
#include "ServerBlock.hpp"

int main()
{
    Parse a("www/a.conf");
    Config::createInstance(a.getHttpStr(), a.getServerLocPairs());
}