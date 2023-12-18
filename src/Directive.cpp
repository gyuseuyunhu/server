#include "Directive.hpp"

bool Directive::checkHttpStr(std::string directive)
{
    std::map<std::string, int>::iterator it;
    it = mHttpDirective.find(directive);
    if (it != mHttpDirective.end())
    {
        if (it->second == 0)
        {
            throw std::runtime_error("duplicate");
        }
        --it->second;
        return true;
    }
    throw std::runtime_error("지시어 없음");
    return false;
}

Directive::Directive()
{
    mHttpDirective["root"] = 1;
    mHttpDirective["index"] = std::numeric_limits<int>::max();
    mHttpDirective["error_page"] = std::numeric_limits<int>::max();
    mHttpDirective["client_max_body_size"] = 1;

    mServerDirective["root"] = 1;
    mServerDirective["index"] = std::numeric_limits<int>::max();
    mServerDirective["error_page"] = std::numeric_limits<int>::max();
    mServerDirective["client_max_body_size"] = 1;
    mServerDirective["listen"] = 1;
    mServerDirective["server_name"] = 1;
    mServerDirective["return"] = 1;

    mLocationDirective["root"] = 1;
    mLocationDirective["index"] = std::numeric_limits<int>::max();
    mLocationDirective["error_page"] = std::numeric_limits<int>::max();
    mLocationDirective["client_max_body_size"] = 1;
    mLocationDirective["return"] = 1;
    mLocationDirective["autoindex"] = 1;
    mLocationDirective["limit_except"] = std::numeric_limits<int>::max();
}