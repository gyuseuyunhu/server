#include "Parse.hpp"
#include "util.hpp"

Parse::Parse(const char *path)
{
    mFile.open(path);
    if (!mFile.is_open())
    {
        throw std::runtime_error("conf file open error");
    }
    mDepth = 0;
    std::string line;
    while (!mFile.eof())
    {
        ftGetLine(mFile, line);
        if (line.size() == 0)
        {
            continue;
        }
        if (line.find("http") != 0)
        {
            throw std::runtime_error("conf file syntax error");
        }
        storeHttpStr(line);
    }
    if (mDepth != 0)
        throw std::runtime_error("conf file syntax error: 괄호 짝 안맞음");
}

void Parse::storeHttpStr(std::string &line)
{
    mHttpStr = "";
    mServerLocPairs.clear();
    line = line.substr(4);
    trimLine(line);
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (mFile.eof() || line[0] != '{')
    {
        throw std::runtime_error("conf file syntax error: 괄호 이상함");
    }
    ++mDepth;
    line = line.substr(1);
    trimLine(line);
    while (!mFile.eof())
    {
        if (line[0] == '}')
        {
            --mDepth;
            if (line.size() == 1)
            {
                return;
            }
            else
            {
                line = line.substr(1);
                trimLine(line);
                if (line.find("http") == 0)
                {
                    storeHttpStr(line);
                    return;
                }
                throw std::runtime_error("일단 에러");
            }
        }
        else if (line.find("server") == 0)
        {
            storeServerStr(line);
        }
        else
        {
            size_t pos;
            if ((pos = line.find("server")) != std::string::npos || (pos = line.find("}")) != std::string::npos)
            {
                mHttpStr += line.substr(0, pos);
                line = line.substr(pos);
                trimLine(line);
            }
            else
            {
                mHttpStr += line;
                ftGetLine(mFile, line);
            }
        }
    }
}

void Parse::storeServerStr(std::string &line)
{
    std::string serverStr;
    LocationVec locationStrVec;
    line = line.substr(6);
    trimLine(line);
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (mFile.eof() || line[0] != '{')
    {
        throw std::runtime_error("conf file syntax error: 괄호 이상함");
    }
    ++mDepth;
    line = line.substr(1);
    trimLine(line);
    while (!mFile.eof())
    {
        if (line[0] == '}')
        {
            --mDepth;
            if (line.size() == 1)
            {
                ftGetLine(mFile, line);
            }
            else
            {
                line = line.substr(1);
                trimLine(line);
            }
            break;
        }
        else if (line.find("location") == 0)
        {
            locationStrVec.push_back(storeLocationStr(line));
        }
        else
        {
            serverStr += line;
            ftGetLine(mFile, line);
        }
    }
    if (locationStrVec.size() == 0)
    {
        throw std::runtime_error("http syntax error: location 블록은 필수적인 값임");
    }
    mServerLocPairs.push_back(std::make_pair(serverStr, locationStrVec));
}

const std::string &Parse::storeLocationStr(std::string &line)
{
    std::string locationStr;
    line = line.substr(8);
    if (!isWhiteSpace(line[0]))
    {
        std::runtime_error("location syntax error: location과 디렉토리 사이의 공백문자 필수");
    }
    trimLine(line);
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (mFile.eof())
    {
        throw std::runtime_error("conf file syntax error: 괄호 이상함");
    }
    if (line.find("/") == std::string::npos)
    {
        throw std::runtime_error("locaiton block syntax error: location 실종");
    }
    size_t pos;
    if ((pos = line.find("{")) == std::string::npos)
    {
        locationStr += line + ";";
        ftGetLine(mFile, line);
    }
    else
    {
        std::string tmp = line.substr(0, pos);
        trimLine(tmp);
        locationStr += tmp + ";";
        line = line.substr(pos);
    }
    while (!mFile.eof() || line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (mFile.eof() || line[0] != '{')
    {
        throw std::runtime_error("conf file syntax error: 괄호 에러");
    }
    ++mDepth;
    line = line.substr(1);
    trimLine(line);
    while (!mFile.eof())
    {
        if (line[0] == '}')
        {
            --mDepth;
            if (line.size() == 1)
            {
                return locationStr;
            }
            else
            {
                line = line.substr(1);
                trimLine(line);
            }
        }
    }
}