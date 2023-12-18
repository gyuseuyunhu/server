#include "util.hpp"

bool isWhiteSpace(const char c)
{
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v')
    {
        return true;
    }
    return false;
}

void trimLine(std::string &line)
{
    size_t begin = line.find_first_not_of(" \n\r\t\f\v");
    size_t end = line.find_last_not_of(" \n\r\t\f\v");
    if (begin == std::string::npos || end == std::string::npos)
    {
        line = "";
        return;
    }
    line = line.substr(begin, end - begin + 1);
}
void deleteComments(std::string &line)
{
    size_t pos = line.find("#");
    if (pos != std::string::npos)
    {
        line = line.substr(0, pos);
    }
}

void ftGetLine(std::ifstream &file, std::string &line)
{
    std::getline(file, line);
    deleteComments(line);
    trimLine(line);
}