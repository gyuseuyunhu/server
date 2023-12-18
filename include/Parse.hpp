#ifndef PARSE_HPP
#define PARSE_HPP

#include <fstream>
#include <iostream>
#include <vector>

class Parse
{
  private:
    typedef std::vector<std::string> LocationVec;
    typedef std::pair<std::string, LocationVec> ServerLocPair;
    std::ifstream mFile;
    int mDepth;
    std::string mHttpStr;
    std::vector<ServerLocPair> mServerLocPairs; // location 블록 경로는 LocationVec의 요소의 첫번째로 저장 ;으로 구분
    void storeHttpStr(std::string &line);
    void storeServerStr(std::string &line);
    const std::string &storeLocationStr(std::string &line);

  public:
    Parse(const char *path);
    ~Parse();
};

#endif

// 처음에 주석(#)이 오거나, 도중에 주석 왔을 때 처리
// "{ }" ";"" 기준으로 잘라서 저장
// 들어온 처음 키워드가 http, server, location 이 세개인지 검사
//

// 보내줄 형식 "지시어 내용 내용 내용"