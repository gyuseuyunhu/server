#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>

class Response
{
  private:
    std::string mStartLine;
    std::string mHead;

  public:
    Response();
    void init(int httpStatusCode, int contentLength);
    const std::string &getStartLine() const;
    const std::string &getHead() const;
};

#endif