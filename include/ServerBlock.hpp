#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "HttpBlock.hpp"
#include <iostream>
#include <vector>

class LocationBlock;

class ServerBlock : public HttpBlock
{
  private:
    // clang-format off
    std::map<std::string, int> checkDirective = {
			{std::string("listen"), 1}, 
			{std::string("server_name"), 1}, 
			{std::string("return"), 1},
			};
		//clang-format on
		//listen의 중복을 허용하지 않고 port로 받음
		const std::string mPort;
		//server_name도 중복이 되지만 허용 안하는것으로
		const std::string mServerName;
  protected:
		// return은 http redirection에 사용하는 지시어
		// 보통  location /old-path {
    //     return 301 /new-path;
    // }
		// 301(영구 이동)나 302(임시 이동)
		// 이런식으로 쓰임
		int mRedirectionCode;
		std::string mRedirectionPath;

};

#endif
