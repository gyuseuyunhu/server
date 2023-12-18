#ifndef PARSE_HPP
#define PARSE_HPP

#include <fstream>
#include <iostream>
#include <vector>

class Parse
{
  private:
    std::ifstream mFile;
    std::string mHttpStr;
    // clang-format off
    std::vector<std::pair<std::string, std::vector<std::string> > > mServerStr;
    // clang-format on
    void storeHttpStr();
    void storeServerStr();
    void storeLocationStr();

  public:
    Parse(const char *path);
};

#endif

// 처음에 주석(#)이 오거나, 도중에 주석 왔을 때 처리
// "{ }" ";"" 기준으로 잘라서 저장
// 들어온 처음 키워드가 http, server, location 이 세개인지 검사
//

// 보내줄 형식 "지시어 내용 내용 내용"