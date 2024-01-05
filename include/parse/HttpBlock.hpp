#ifndef HTTPBLOCK_HPP
#define HTTPBLOCK_HPP

#include <iostream>
#include <limits>
#include <map>
#include <vector>

class HttpBlock
{
  protected:
    std::string mRoot;
    std::vector<std::string> mIndexs;
    std::map<int, std::string> mErrorPages;
    unsigned int mClientMaxBodySize;

  public:
    HttpBlock(const std::string &root, const std::vector<std::string> &index,
              const std::map<int, std::string> &errorpage, unsigned int clientMaxBodySize);
    HttpBlock(const HttpBlock &other);
    HttpBlock &operator=(const HttpBlock &rhs);

    const std::string &getRoot() const;
    const std::vector<std::string> &getIndexs() const;
    const std::map<int, std::string> &getErrorPages() const;
    unsigned int getClientMaxBodySize() const;
    // todo : friend 없애야함
    friend std::ostream &operator<<(std::ostream &os, const HttpBlock &httpBlock);
};

#endif