#ifndef READREQUESTEVENT_HPP
#define READREQUESTEVENT_HPP

#include "AEvent.hpp"

class ReadRequestEvent : public AEvent
{
  private:
    static const int BUFFER_SIZE = 1024; // todo 테스트를 통해 적절한 값 찾기
    std::string mStringBuffer;

  public:
    ReadRequestEvent(const Server &server, int clientSocket);
    virtual ~ReadRequestEvent();
    virtual int handle();
    void makeResponse();
};

#endif