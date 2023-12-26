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
    unsigned int getPort() const;
    const std::string &getServerName() const;
    unsigned int getRedirectionCode() const;
    const std::string &getRedirectionPath() const;
    friend std::ostream &operator<<(std::ostream &os, const ServerBlock &serverBlock);
};

#endif
