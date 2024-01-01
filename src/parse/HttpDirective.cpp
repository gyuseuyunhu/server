#include "HttpDirective.hpp"

HttpDirective::HttpDirective() : ADirective()
{
    initDirectiveMap();
}

void HttpDirective::initDirectiveMap()
{
    mDirective["root"] = 1;
    mDirective["index"] = std::numeric_limits<int>::max();
    mDirective["error_page"] = std::numeric_limits<int>::max();
    mDirective["client_max_body_size"] = 1;
}
