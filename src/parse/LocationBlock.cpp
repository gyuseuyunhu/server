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

std::ostream &operator<<(std::ostream &os, const LocationBlock &locationBlock)
{
    os << "LocationPath : " << locationBlock.mLocationPath << std::endl;
    os << "root : " << locationBlock.mRoot << std::endl;

    std::vector<std::string>::const_iterator it = locationBlock.mIndexs.begin();

    os << "index : ";
    for (; it != locationBlock.mIndexs.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "errorPages : ";
    std::map<int, std::string>::const_iterator it2 = locationBlock.mErrorPages.begin();
    for (; it2 != locationBlock.mErrorPages.end(); ++it2)
    {
        os << it2->first << " ";
        os << it2->second << " ";
    }
    os << std::endl;

    os << "clientMaxBodySize : " << locationBlock.mClientMaxBodySize << std::endl;

    os << "port : " << locationBlock.mPort << std::endl;
    os << "serverName : " << locationBlock.mServerName << std::endl;
    os << "redirectionPath : " << locationBlock.mRedirectionPath << std::endl;

    os << "isAutoIndex : " << std::boolalpha << locationBlock.mIsAutoIndex << std::endl;
    os << "isAllowedGet: " << std::boolalpha << locationBlock.mIsAllowedGet << std::endl;
    os << "isAllowedPost : " << std::boolalpha << locationBlock.mIsAllowedPost << std::endl;
    os << "isAllowedDelete : " << std::boolalpha << locationBlock.mIsAllowedDelete << std::endl;
    os << "cgiPath : " << locationBlock.mCgiPath << std::endl;
    os << "cgiExtension : " << locationBlock.mCgiExtension << std::endl;
    os << "cgiUploadDir : " << locationBlock.mCgiUploadDir << std::endl;
    return os;
}
