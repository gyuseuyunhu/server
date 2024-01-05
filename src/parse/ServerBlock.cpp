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

unsigned int ServerBlock::getPort() const
{
    return mPort;
}

const std::string &ServerBlock::getServerName() const
{
    return mServerName;
}

unsigned int ServerBlock::getRedirectionCode() const
{
    return mRedirectionCode;
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
    os << "redirectionCode : " << serverBlock.mRedirectionCode << "    ";
    os << "redirectionPath : " << serverBlock.mRedirectionPath << std::endl;

    return os;
}
