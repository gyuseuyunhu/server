#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "ConnectionEnum.hpp"
#include <iostream>

class Response
{
  private:
    std::string mStartLine;
    std::string mHead;
    std::string mBody;
    eConnectionStatus mConnectionStatus;

  public:
    Response();
    void setStartLine(int httpStatusCode);
    void addHead(const std::string &key, const int value);
    void addHead(const std::string &key, const std::string &value);
    void setBody(const std::string &body);
    const std::string toStr() const;
    const std::string &getStartLine() const;
    const std::string &getHead() const;
    eConnectionStatus getConnectionStatus() const;
    void setConnectionClose();
};

#endif