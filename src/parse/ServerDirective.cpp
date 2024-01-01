#include "ServerDirective.hpp"

ServerDirective::ServerDirective() : ADirective()
{
    initDirectiveMap();
}

void ServerDirective::initDirectiveMap()
{
    mDirective["root"] = 1;
    mDirective["index"] = std::numeric_limits<int>::max();
    mDirective["error_page"] = std::numeric_limits<int>::max();
    mDirective["client_max_body_size"] = 1;
    mDirective["listen"] = 1;
    mDirective["server_name"] = 1;
    mDirective["return"] = 1;
}
