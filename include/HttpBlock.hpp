#ifndef HTTPBLOCK_HPP
#define HTTPBLOCK_HPP

#include <iostream>
#include <limits>
#include <map>
#include <vector>

class HttpBlock
{
  private:
    // clang-format off
    std::map<std::string, int> checkDirective = {
			{std::string("root"), 1}, 
			{std::string("index"), std::numeric_limits<int>::max()}, 
			{std::string("error_page"), std::numeric_limits<int>::max()}, 
			{std::string("client_max_body_size"), 1}, 
			};
		//clang-format on
  protected:
    std::string mRoot;
		std::vector<std::string> mIndexs;
		std::vector<std::string> mErrorPages;
		unsigned int mClientMaxBodySize;
};

#endif