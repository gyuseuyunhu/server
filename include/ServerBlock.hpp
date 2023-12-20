#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "HttpBlock.hpp"
#include <iostream>

class ServerBlock : public HttpBlock
{
    // port 기본값 80, serverName "", return은 기본값이 없음
    // 우리가 정해놓을까? redirectionCode = 0; path = "";
    // listen의 중복을 허용하지 않고 port로 받음

  protected:
    // return은 http redirection에 사용하는 지시어
    // 보통  location /old-path {
    //     return 301 /new-path;
    // 어디까지 들어올수 있나..
    // 301
    // }
    // 301(영구 이동)나 302(임시 이동)
    // 이런식으로 쓰임
    unsigned int mPort;
    // server_name도 중복이 되지만 허용 안하는것으로
    std::string mServerName;
    unsigned int mRedirectionCode;
    std::string mRedirectionPath;

  public:
    ServerBlock(HttpBlock httpBlock, unsigned int port, std::string serverName, unsigned int redirectionCode,
                std::string redirectionPath);
    ServerBlock(const ServerBlock &other);
    ServerBlock &operator=(const ServerBlock &rhs);
};

#endif
