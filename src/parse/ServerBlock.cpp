#include "ServerBlock.hpp"

ServerBlock::ServerBlock(const HttpBlock &httpBlock, unsigned int port, const std::string &serverName,
                         const std::string &redirectionPath)
    : HttpBlock(httpBlock), mPort(port), mServerName(serverName), mRedirectionPath(redirectionPath)
{
}

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
        mRoot = rhs.mRoot;
        mIndexs = rhs.mIndexs;
        mErrorPages = rhs.mErrorPages;
        mClientMaxBodySize = rhs.mClientMaxBodySize;

        mPort = rhs.mPort;
        mServerName = rhs.mServerName;
        mRedirectionPath = rhs.mRedirectionPath;
    }
    return *this;
}

unsigned int ServerBlock::getPort() const
{
    return mPort;
}

const std::string &ServerBlock::getServerName() const
{
    return mServerName;
}

const std::string &ServerBlock::getRedirectionPath() const
{
    return mRedirectionPath;
}
