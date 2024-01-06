#include "util.hpp"

int nonBlockOpen(const char *str, int flag)
{
    int fd = open(str, flag);
    if (fd == -1)
    {
        return -1;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    return fd;
}

std::string trim(const std::string &str)
{
    std::size_t first = str.find_first_not_of(" ");
    std::size_t last = str.find_last_not_of(" ");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

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