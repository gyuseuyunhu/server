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
