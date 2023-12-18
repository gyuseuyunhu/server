#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <map>
#include <string>
#include <vector>

// 1. 문제 directive을 하나를 생성하고 여러개의 server_block이 참고하면
//    문제가 생김
// 2. 문제 mHttpDirective에만 root가 있음("root", 1);
//    http block에서 1을 감소시키고
//    즉 server에서 root을 재지정할경우 int 값이 0이 되면서 에러가 난다.
// 해결법 : 1. 원복
//        2. server_block의 생성자에 directive을 넣어준다 단점은 복사가 많다
//        3. 각각의 block을 만든 후 directive 메서드로 reset 함수를 만들어서 map의 value reset
//            단점 : reset 빼먹으면 안 됨
//        Directive.checkStr(enum blockDepth, std::string str)
//        {
//           while (str벡터 끝까지)
//          {
//	           if (mHttpDirective.find(http) == false)
//              {
//                 throw;
//              }
//          }
//        }
//        location, server, http 지시어 중복, 없는 지시어정도를 확인
//        단점은 시간복잡도 한번 훑었는데 하는일이 에러체크만 ...
//        4. const로 해놓고 복사한것을 쓰자 단점: 내생각에는 const_cast가 필요할듯?
//        그 함수를 config
//        config가 directive을 가지고 있고
//        config.checkstr(enum blockDepth, std::string str)
//        {
//             if (blockDepth == 1)
//             {
//                  mDirec
//              }
//        }
class Directive
{
  private:
    // clang-format off
    std::map<std::string, int> mHttpDirective;
		std::map<std::string, int> mServerDirective;
		std::map<std::string, int> mLocationDirective;
    // clang-format on
  public:
    bool checkHttpStr(std::string directive);
    Directive();
};

#endif