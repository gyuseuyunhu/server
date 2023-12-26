#include "ADirective.hpp"

void ADirective::checkDirective(const std::string &str)
{
    std::map<std::string, int>::iterator it;

    it = mDirective.find(str);
    if (it != mDirective.end())
    {
        if (it->second == 0)
        {
            throw std::runtime_error("Error : ADirective::checkDirective() 지시어 중복\n" + str);
        }
        --it->second;
    }
    else
    {
        throw std::runtime_error("Error : ADirective::checkDirective() 지시어 없음\n" + str);
    }
}