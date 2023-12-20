#include "ServerBlock.hpp"

ServerBlock::ServerBlock(HttpBlock httpBlock, unsigned int port, std::string serverName, unsigned int redirectionCode,
                         std::string redirectionPath)
    : HttpBlock(httpBlock), mPort(port), mServerName(serverName), mRedirectionCode(redirectionCode),
      mRedirectionPath(redirectionPath)
{
}

// todo 이게 맞는지 확인 필요
ServerBlock::ServerBlock(const ServerBlock &other) : HttpBlock(other)
{
    if (this != &other)
    {
        *this = other;
    }
}

ServerBlock &ServerBlock::operator=(const ServerBlock &rhs)
{
    if (this != &rhs)
    {
        // 부모
        mRoot = rhs.mRoot;
        mIndexs = rhs.mIndexs;
        mErrorPages = rhs.mErrorPages;
        mClientMaxBodySize = rhs.mClientMaxBodySize;

        mPort = rhs.mPort;
        mServerName = rhs.mServerName;
        mRedirectionCode = rhs.mRedirectionCode;
        mRedirectionPath = rhs.mRedirectionPath;
    }
    return *this;
}
