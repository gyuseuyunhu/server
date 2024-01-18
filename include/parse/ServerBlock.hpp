#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "HttpBlock.hpp"
#include <iostream>

class ServerBlock : public HttpBlock
{
  protected:
    unsigned int mPort;
    std::string mServerName;
    std::string mRedirectionPath;

  public:
    ServerBlock(const HttpBlock &httpBlock, unsigned int port, const std::string &serverName,
                const std::string &redirectionPath);
    ServerBlock(const ServerBlock &other);
    ServerBlock &operator=(const ServerBlock &rhs);
    unsigned int getPort() const;
    const std::string &getServerName() const;
    const std::string &getRedirectionPath() const;
};

#endif
