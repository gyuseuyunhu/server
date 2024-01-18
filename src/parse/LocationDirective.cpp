#include "LocationDirective.hpp"

LocationDirective::LocationDirective() : ADirective()
{
    initDirectiveMap();
}

void LocationDirective::initDirectiveMap()
{
    mDirective["path"] = 1;
    mDirective["root"] = 1;
    mDirective["index"] = std::numeric_limits<int>::max();
    mDirective["error_page"] = std::numeric_limits<int>::max();
    mDirective["client_max_body_size"] = 1;
    mDirective["return"] = 1;
    mDirective["autoindex"] = 1;
    mDirective["limit_except"] = std::numeric_limits<int>::max();
    mDirective["cgi_extension"] = 1;
    mDirective["cgi_path"] = 1;
    mDirective["cgi_upload_dir"] = 1;
}
