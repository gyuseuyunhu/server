
#include "HttpBlock.hpp"

HttpBlock::HttpBlock(std::string root, std::vector<std::string> indexs, std::vector<std::string> errorpages,
                     unsigned int clientMaxBodySize)
    : mRoot(root), mIndexs(indexs), mErrorPages(errorpages), mClientMaxBodySize(clientMaxBodySize)
{
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