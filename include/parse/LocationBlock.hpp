#ifndef LOCATION_BLOCK_HPP
#define LOCATION_BLOCK_HPP

#include "ServerBlock.hpp"
#include <iostream>

class LocationBlock : public ServerBlock
{
  private:
    std::string mLocationPath;
    bool mIsAutoIndex;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;
    std::string mCgiPath;
    std::string mCgiExtension;
    std::string mCgiUploadDir;

  public:
    LocationBlock(const ServerBlock &serverBlock, const std::string &locationPath, bool isAutoIndex, bool isAllowedGet,
                  bool isAllowedPost, bool isAllowedDelete, const std::string &cgiPath, const std::string &cgiExtension,
                  const std::string &cgiUploadDir);
    LocationBlock(const ServerBlock &serverBlock);
    LocationBlock &operator=(const LocationBlock &rhs);
    const std::string &getLocationPath() const;
    bool isAutoIndex() const;
    bool isAllowedGet() const;
    bool isAllowedPost() const;
    bool isAllowedDelete() const;
    const std::string &getCgiPath() const;
    const std::string &getCgiExtension() const;
    const std::string &getCgiUploadDir() const;
    friend std::ostream &operator<<(std::ostream &os, const LocationBlock &locationBlock);
};

#endif