#include "Parse.hpp"
#include "util.hpp"

Parse::Parse(const char *path)
{
    mFile.open(path);
    if (!mFile.is_open())
    {
        throw(std::runtime_error("conf file opne error"));
    }
    std::string line;
    ftGetLine(mFile, line);
    // 지시어, {}, ; 찾기
    // 지시어 http, server, location이 순서대로 오지 않으면 에러
}