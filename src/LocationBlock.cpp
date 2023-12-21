#include "LocationBlock.hpp"
LocationBlock::LocationBlock(ServerBlock serverBlock, std::string locationPath, bool isAutoIndex, bool isAllowedGet,
                             bool isAllowedPost, bool isAllowedDelete)
    : ServerBlock(serverBlock), mLocationPath(locationPath), mIsAutoIndex(isAutoIndex), mIsAllowedGet(isAllowedGet),
      mIsAllowedPost(isAllowedPost), mIsAllowedDelete(isAllowedDelete)
{
}

LocationBlock &LocationBlock::operator=(const LocationBlock &rhs)
{
    if (this != &rhs)
    {
        // 할아버지
        mRoot = rhs.mRoot;
        mIndexs = rhs.mIndexs;
        mErrorPages = rhs.mErrorPages;
        mClientMaxBodySize = rhs.mClientMaxBodySize;

        // 아버지
        mPort = rhs.mPort;
        mServerName = rhs.mServerName;
        mRedirectionCode = rhs.mRedirectionCode;
        mRedirectionPath = rhs.mRedirectionPath;

        mLocationPath = rhs.mLocationPath;
        mIsAutoIndex = rhs.mIsAutoIndex;
        mIsAllowedGet = rhs.mIsAllowedGet;
        mIsAllowedPost = rhs.mIsAllowedPost;
        mIsAllowedDelete = rhs.mIsAllowedDelete;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, LocationBlock &locationBlock)
{
    os << "root : " << locationBlock.mRoot << std::endl;

    std::vector<std::string>::iterator it = locationBlock.mIndexs.begin();

    os << "index : ";
    for (; it != locationBlock.mIndexs.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "errorPages : ";
    it = locationBlock.mErrorPages.begin();
    for (; it != locationBlock.mErrorPages.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "clientMaxBodySize : " << locationBlock.mClientMaxBodySize << std::endl;

    os << "port : " << locationBlock.mPort << std::endl;
    os << "serverName : " << locationBlock.mServerName << std::endl;
    os << "redirectionCode : " << locationBlock.mRedirectionCode << "    ";
    os << "redirectionPath : " << locationBlock.mRedirectionPath << std::endl;

    os << "LocationPath : " << locationBlock.mLocationPath << std::endl;
    os << "isAutoIndex : " << std::boolalpha << locationBlock.mIsAutoIndex << std::endl;
    os << "isAllowedGet: " << std::boolalpha << locationBlock.mIsAllowedGet << std::endl;
    os << "isAllowedPost : " << std::boolalpha << locationBlock.mIsAllowedPost << std::endl;
    os << "isAllowedDelete : " << std::boolalpha << locationBlock.mIsAllowedDelete << std::endl;
    return os;
}
