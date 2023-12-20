#include "ADirective.hpp"

void ADirective::checkDirective(const std::string &str)
{
    std::map<std::string, int>::iterator it;
    it = mDirective.find(str);
    if (it != mDirective.end())
    {
        if (it->second == 0)
        {
            throw std::runtime_error("duplicate");
        }
        --it->second;
    }
    throw std::runtime_error("지시어 없음");
}