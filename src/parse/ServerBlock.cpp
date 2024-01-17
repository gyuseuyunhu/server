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

std::ostream &operator<<(std::ostream &os, const ServerBlock &serverBlock)
{
    os << "root : " << serverBlock.mRoot << std::endl;

    std::vector<std::string>::const_iterator it = serverBlock.mIndexs.begin();

    os << "index : ";
    for (; it != serverBlock.mIndexs.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "errorPages : ";
    std::map<int, std::string>::const_iterator it2 = serverBlock.mErrorPages.begin();
    for (; it2 != serverBlock.mErrorPages.end(); ++it2)
    {
        os << it2->first << " ";
        os << it2->second << " ";
    }
    os << std::endl;

    os << "clientMaxBodySize : " << serverBlock.mClientMaxBodySize << std::endl;

    os << "port : " << serverBlock.mPort << std::endl;
    os << "serverName : " << serverBlock.mServerName << std::endl;
    os << "redirectionPath : " << serverBlock.mRedirectionPath << std::endl;

    return os;
}
