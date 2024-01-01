
#include "HttpBlock.hpp"

HttpBlock::HttpBlock(std::string root, std::vector<std::string> indexs, std::vector<std::string> errorpages,
                     unsigned int clientMaxBodySize)
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

const std::vector<std::string> &HttpBlock::getErrorPages() const
{
    return mErrorPages;
}

unsigned int HttpBlock::getClientMaxBodySize() const
{
    return mClientMaxBodySize;
}

std::ostream &operator<<(std::ostream &os, const HttpBlock &httpBlock)
{
    os << "root : " << httpBlock.mRoot << std::endl;

    std::vector<std::string>::const_iterator it = httpBlock.mIndexs.begin();

    os << "index : ";
    for (; it != httpBlock.mIndexs.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "errorPages : ";
    it = httpBlock.mErrorPages.begin();
    for (; it != httpBlock.mErrorPages.end(); ++it)
    {
        os << *it << " ";
    }
    os << std::endl;

    os << "clientMaxBodySize : " << httpBlock.mClientMaxBodySize << std::endl;

    return os;
}
