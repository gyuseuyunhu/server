#ifndef LOCATION_BLOCK_HPP
#define LOCATION_BLOCK_HPP

#include "ServerBlock.hpp"
#include <iostream>

class LocationBlock : public ServerBlock
{
  private:
    // clang-format off
    std::map<std::string, int> checkDirective = {
			{std::string("autoindex"), 1}, 
			{std::string("limit_except"), std::numeric_limits<int>::max()}, 
		};
    // clang-format on
    bool mAutoIndex;
    // limit_except는 기존의 nginx랑 다르게 받음
    // limit_except allow GET;
    // limit_except deny POST;
    // 처럼 중복으로 올 수 있지만 한줄에 여러개 쓰는것, {}로 ip등 세부지정은 안되는것으로
    // 기본값은 허용
    bool mIsAllowedGet;
    bool mIsAllowedPost;
    bool mIsAllowedDelete;
};

#endif