
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

std::ostream &operator<<(std::ostream &os, HttpBlock &httpBlock)
{
    os << "root : " << httpBlock.mRoot << std::endl;

    std::vector<std::string>::iterator it = httpBlock.mIndexs.begin();

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
