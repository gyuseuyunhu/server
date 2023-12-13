#include "Config.hpp"

void Config::trimLine(std::string &line)
{
    size_t begin = line.find_first_not_of(" \n\r\t\f\v");
    size_t end = line.find_last_not_of(" \n\r\t\f\v");
    if (begin == std::string::npos || end == std::string::npos)
    {
        line = line.substr(begin, end - begin + 1);
    }
}

void Config::checkHttpBlock()
{
    std::string line;
    while (!mFile.eof())
    {
        std::getline(mFile, line);
        deleteComments(line);
        trimLine(line);
        if (line.size() == 0)
            continue;
        // line에 http 헤더가 있는지 검사하고 { } 블록이 들어왔는지 검사
    }
    throw(std::runtime_error("http block need"));
}

Config::Config(const char *path)
{
    mFile.open(path);
    if (!mFile.is_open())
    {
        throw(std::runtime_error("conf file opne error"));
    }
    parse();
}
// http {
// 	server {
//  	       listen       host:8080
//    	     server_name  localhost

//	         location / {
//  	           root   html
//    	         index  index.html index.htm
//       		 }
//	}
//}

// {} 이거는 블락만 쓰는데 http server location만 쓰는 거지?
// location의 http 허용 메서드 지시어도 쓰는데 우린 안 할거임
// 일단 그렇게 생각하면 될 듯

// location / Delete {} <- 요거 안쓴다고?
// parse을 어떻게 짜야하는지 고민중 ...
// http { 이게 필수
// { 이거 하나 만날때 마다 depth 변수를 증가시킬까...
//

void Config::deleteComments(std::string &line)
{
    size_t pos = line.find("#");
    if (pos != std::string::npos)
    {
        line = line.substr(0, pos);
    }
}

void Config::parse()
{
    std::string line;
    checkHttpBlock();
    while (!mFile.eof())
    {
        std::getline(mFile, line);
        deleteComments(line);
        trimLine(line);
        if (line.size() == 0)
        {
            continue;
        }
    }
}

void Config::parseServer()
{
    mServerBlocks.push_back(ServerBlock());
}

void Config::errorCheck()
{
}

// fin