#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include <iostream>
#include <vector>

class LocationBlock;

class ServerBlock
{
  private:
    // listen [Ip주소][:mPort];
    // nginx 해보니깐 listen 지시어 중복 가능, port 중복 가능, root는 중복이 불가능 // 노노 포트 덮어씌우고 하나만 저장
    // listen 80; listen 8080; 두개가 들어오면 localhost:80, localhost:8080이 둘 다 접속이 가능하다 // 아니다
    // 8080만가능하다 즉 port는 여러개를 들고 있어야 할듯
    const std::string mServerName; // 안들어올수도 있음 server_name
    const std::string mHost;
    const unsigned int mPort;
    const std::string mRoot;
    const std::vector<LocationBlock> mLocationBlocks;

  public:
    ServerBlock();
};

#endif
