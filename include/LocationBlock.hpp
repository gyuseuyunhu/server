#ifndef LOCATION_BLOCK_HPP
#define LOCATION_BLOCK_HPP

#include "ServerBlock.hpp"
#include <iostream>

class LocationBlock : public ServerBlock
{
  private:
    // limit_except는 기존의 nginx랑 다르게 받음
    // limit_except allow GET;
    // limit_except deny POST;
    // 처럼 중복으로 올 수 있지만 한줄에 여러개 쓰는것, {}로 ip등 세부지정은 안되는것으로
    // 기본값은 허용
    std::string mLocationPath;
    bool mIsAutoIndex;
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;

  public:
    LocationBlock(ServerBlock serverBlock, std::string locationPath, bool isAutoIndex, bool isAllowedGet,
                  bool isAllowedPost, bool isAllowedDelete);
    LocationBlock &operator=(const LocationBlock &rhs);
    std::string getLocationPath() const;
    bool isAutoIndex() const;
    bool isAllowedGet() const;
    bool isAllowedPost() const;
    bool isAllowedDelete() const;
    friend std::ostream &operator<<(std::ostream &os, const LocationBlock &locationBlock);
};

#endif