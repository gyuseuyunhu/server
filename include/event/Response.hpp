#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>

class Response
{
  private:
    std::string mStartLine;
    std::string mHead;
    std::string mBody;

  public:
    Response();
    void init(int httpStatusCode, int contentLength);
    void addHead(const std::string &key, const std::string &value);
    void setBody(const std::string &body);
    const std::string toStr() const;
    const std::string &getStartLine() const;
    const std::string &getHead() const;
};

#endif