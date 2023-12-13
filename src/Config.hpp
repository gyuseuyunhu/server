#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "LocationBlock.hpp"
#include "ServerBlock.hpp" // Include the header file for ServerBlock class
#include <fstream>
#include <iostream>
#include <map>
#include <stack> //TODO - {} 확인용, 필요없으면 삭제
#include <vector>

class Config
{
  private:
    const std::ifstream mFile; // path로 연 파일
    std::vector<ServerBlock> mServerBlocks;

    void parse();
    void parseServer();
    void parseLocation();
    void errorCheck();
    bool isWhiteLine(std::string &line);

  public:
    Config(std::string &path);
};
// parse을 언제 호출할까요?>
// 1. config 생성자에서..
// 2. createConfig 함수를 사용해서
// 3. 다른의견 있으면 적어줘용
// 그냥 parse 함수 호출 하면 안돼요?
// 4. 그냥 parse 함수 호출
// 	> config 클래스를 사용하려면 parse가 필수적인데 사용자가 parse를 호출을 해줘야 해서...
// 그럼 생성자에서 parse 호출 하자
// > 근데 그건 생성자에서 error throw을 던져야 하는데....
// 근데 컨피그 잘못되면 webserv 실행도 안될 거니까 throw로 처리해도 될듯
// 바로 프로그램 종료되어야 되잔허
// 맞나... 맞다...
// server Maneger가 있는데 여기서 config 객체를 받아서 서버를 생성해줌
#endif
// 포트 벡터가 필요할까?
// server name 벡터는?
//  root 벡터는?
//  location block
//  serverblokc 벡터는?

// 서버 벡터는 필요할 것 같고, 포트 백터는 음

// 문법 에러가 port가 같은 번호로 들어왔으면 에러임?
// 그건 아니었음 { } 짝이 안맞는 거// 지시어가 없는 거
// ;  이거 체크는 모르겠는데용
// 뭐가 문법에러야??? 예시 부탁해요
// 모르겠다

// 저는 port 중복도 확인하면 좋을거 같아요 - 어차피 하나만 받기로 했으니깐 ㅇㅈ
// 그러면 같은 포트이면 bind에서 확인하는거야?
// nfig(std::string &path);
