
#include "HttpBlock.hpp"

HttpBlock::HttpBlock(const std::string &root, const std::vector<std::string> &indexs,
                     const std::map<int, std::string> &errorpages, unsigned int clientMaxBodySize)
    : mRoot(root), mIndexs(indexs), mErrorPages(errorpages), mClientMaxBodySize(clientMaxBodySize)
{
}

HttpBlock::HttpBlock(const HttpBlock &other)
{
    if (this != &other)
    {
        *this = other;
    }
}

HttpBlock &HttpBlock::operator=(const HttpBlock &rhs)
{
    if (this != &rhs)
    {
        mRoot = rhs.mRoot;
        mIndexs = rhs.mIndexs;
        mErrorPages = rhs.mErrorPages;
        mClientMaxBodySize = rhs.mClientMaxBodySize;
    }
    return *this;
}

const std::string &HttpBlock::getRoot() const
{
    return mRoot;
}

const std::vector<std::string> &HttpBlock::getIndexs() const
{
    return mIndexs;
}

const std::map<int, std::string> &HttpBlock::getErrorPages() const
{
    return mErrorPages;
}

unsigned int HttpBlock::getClientMaxBodySize() const
{
    return mClientMaxBodySize;
}
