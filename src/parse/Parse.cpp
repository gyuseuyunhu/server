#include "Parse.hpp"
#include "util.hpp"
#include <cstring>

void Parse::skipBeforeHttp(std::string &line)
{
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (mFile.eof())
    {
        return;
    }
    if (line.find("http") != 0)
    {
        throw std::runtime_error("Error Parse::skipBeforeHttp(): http 지시어가 없습니다.\n" + line);
    }
}

void Parse::checkOpenBrace(std::string &line)
{
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }
    if (line[0] != '{')
    {
        throw std::runtime_error("Error Parse::checkOpenBrace(): '{' 이 없습니다.\n" + line);
    }
    ++mDepth;
    line = line.substr(1);
    trimLine(line);
}

void Parse::checkCloseBrace(std::string &line)
{
    --mDepth;
    line = line.substr(1);
    trimLine(line);
}

void Parse::skipDirective(std::string &line, int num)
{
    line = line.substr(num);
    trimLine(line);
    checkOpenBrace(line);
}

void Parse::checkLocationPath(std::string &line, std::string &locationStr)
{
    while (!mFile.eof() && line.size() == 0)
    {
        ftGetLine(mFile, line);
    }

    if (line.size() == 0)
    {
        throw std::runtime_error("Error Parse::checkLocationPath(): Location path가 없습니다.");
    }

    size_t pos;
    if ((pos = line.find("{")) != std::string::npos)
    {
        std::string tmp = line.substr(0, pos);
        trimLine(tmp);
        if (tmp.size() == 0)
        {
            throw std::runtime_error("Error Parse::checkLocationPath(): Location Path가 없습니다.");
        }
        locationStr += "path " + tmp + ";";
        line = line.substr(pos);
    }
    else
    {
        locationStr += "path " + line + ";";
        ftGetLine(mFile, line);
    }
}

void Parse::parseElseLine(std::string &str, std::string &line)
{
    size_t pos;
    if ((pos = line.find(";")) != std::string::npos)
    {
        str += line.substr(0, pos + 1);
        line = line.substr(pos + 1);
        trimLine(line);
    }
    else if ((pos = line.find("}")) != std::string::npos)
    {
        str += line.substr(0, pos);
        line = line.substr(pos);
        trimLine(line);
    }
    else
    {
        if (line.size() != 0)
        {
            str += line + " ";
        }
        ftGetLine(mFile, line);
    }
}

Parse::Parse(const char *path)
{
    mFile.open(path);
    if (!mFile.is_open())
    {
        throw std::runtime_error("Error Parse::Parse(): 파일이 열리지 않습니다.");
    }
    mDepth = 0;
    std::string line;
    while (!mFile.eof() || line.size() != 0)
    {
        skipBeforeHttp(line);
        if (mFile.eof() && line.size() == 0)
            break;
        storeHttpStr(line);
    }
    if (mDepth != 0)
        throw std::runtime_error("Error Parse::Parse(): 괄호가 닫히지 않았습니다.");
}

Parse::~Parse()
{
    mServerInfoStrs.clear();
}

void Parse::storeHttpStr(std::string &line)
{
    mHttpStr = "";
    mServerInfoStrs.clear();

    skipDirective(line, std::strlen("http"));
    while (!mFile.eof() || line.size() != 0)
    {
        if (line.size() == 0)
        {
            ftGetLine(mFile, line);
        }
        if (line[0] == '}')
        {
            checkCloseBrace(line);
            return;
        }
        else if (line.find("server") == 0)
        {
            storeServerStr(line);
        }
        else
        {
            parseElseLine(mHttpStr, line);
        }
    }
}

void Parse::storeServerStr(std::string &line)
{
    std::string serverStr;
    std::vector<std::string> locationStrVec;

    skipDirective(line, std::strlen("server"));

    while (!mFile.eof() || line.size() != 0)
    {
        if (line.size() == 0)
        {
            ftGetLine(mFile, line);
        }
        if (line[0] == '}')
        {
            checkCloseBrace(line);
            mServerInfoStrs.push_back(ServerInfoStr(serverStr, locationStrVec));
            return;
        }
        else if (line.find("location") == 0)
        {
            locationStrVec.push_back(storeLocationStr(line));
        }
        else
        {
            parseElseLine(serverStr, line);
        }
    }
}

std::string Parse::storeLocationStr(std::string &line)
{
    std::string locationStr;
    line = line.substr(8);
    if (!isWhiteSpace(line[0]))
    {
        throw std::runtime_error("");
    }
    trimLine(line);
    checkLocationPath(line, locationStr);
    checkOpenBrace(line);

    while (!mFile.eof() || line.size() != 0)
    {
        if (line.size() == 0)
        {
            ftGetLine(mFile, line);
        }
        if (line[0] == '}')
        {
            checkCloseBrace(line);
            break;
        }
        else
        {
            parseElseLine(locationStr, line);
        }
    }
    return locationStr;
}

const std::string &Parse::getHttpStr() const
{
    return mHttpStr;
}

const std::vector<ServerInfoStr> &Parse::getServerInfoStrs() const
{
    return mServerInfoStrs;
}