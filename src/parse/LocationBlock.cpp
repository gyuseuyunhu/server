#include "LocationBlock.hpp"
LocationBlock::LocationBlock(const ServerBlock &serverBlock, const std::string &locationPath, bool isAutoIndex,
                             bool isAllowedGet, bool isAllowedPost, bool isAllowedDelete, const std::string &cgiPath,
                             const std::string &cgiExtension, const std::string &cgiUploadDir)
    : ServerBlock(serverBlock), mLocationPath(locationPath), mIsAutoIndex(isAutoIndex), mIsAllowedGet(isAllowedGet),
      mIsAllowedPost(isAllowedPost), mIsAllowedDelete(isAllowedDelete), mCgiPath(cgiPath), mCgiExtension(cgiExtension),
      mCgiUploadDir(cgiUploadDir)
{
}
LocationBlock::LocationBlock(const ServerBlock &serverBlock)
    : ServerBlock(serverBlock), mLocationPath(""), mIsAutoIndex(false), mIsAllowedGet(true), mIsAllowedPost(true),
      mIsAllowedDelete(false), mCgiUploadDir("/")
{
}

LocationBlock &LocationBlock::operator=(const LocationBlock &rhs)
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

        mLocationPath = rhs.mLocationPath;
        mIsAutoIndex = rhs.mIsAutoIndex;
        mIsAllowedGet = rhs.mIsAllowedGet;
        mIsAllowedPost = rhs.mIsAllowedPost;
        mIsAllowedDelete = rhs.mIsAllowedDelete;
        mCgiPath = rhs.mCgiPath;
        mCgiExtension = rhs.mCgiExtension;
        mCgiUploadDir = rhs.mCgiUploadDir;
    }
    return *this;
}

const std::string &LocationBlock::getLocationPath() const
{
    return mLocationPath;
}

bool LocationBlock::isAutoIndex() const
{
    return mIsAutoIndex;
}

bool LocationBlock::isAllowedGet() const
{
    return mIsAllowedGet;
}

bool LocationBlock::isAllowedPost() const
{
    return mIsAllowedPost;
}

bool LocationBlock::isAllowedDelete() const
{
    return mIsAllowedDelete;
}

const std::string &LocationBlock::getCgiPath() const
{
    return mCgiPath;
}

const std::string &LocationBlock::getCgiExtension() const
{
    return mCgiExtension;
}

const std::string &LocationBlock::getCgiUploadDir() const
{
    return mCgiUploadDir;
}
