#include "Config.hpp"

Config::Config(std::string &path)
{
    mFile.open(path);
    if (!mFile)
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

bool Config::isWhiteLine(std::string &line)
{
    if (line.find_first_not_of(" \n\r\t\f\v") == std::string::npos)
        return true;
    return false;
}

void Config::parse()
{
    bool isFirstRead = false;
    // 이제 getline으로 받으면서 parsing 하나요? - nae
    // 해주세요
    std::string line;
    while (!mFile.eof())
    {
        std::getline(mFile, line);
        // whitespace 이후에 #이 오는 경우 처리가 안 됨
        if (isWhiteLine(line) || line.find_first_of('#') == 0 || line.size() == 0)
        {
            continue;
        }

        if (line.find_first_of('http') != std::npos && isFirstRead == false)
        {
            throw(std::runtime_error("http block need"));
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