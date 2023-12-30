#ifndef READEVENT_HPP
#define READEVENT_HPP

#include "AEvent.hpp"

class ReadEvent : public AEvent
{
  private:
    const int mClientSocket;
    static const int BUFFER_SIZE = 1024; // todo 테스트를 통해 적절한 값 찾기
    char mBuffer[BUFFER_SIZE];
    std::string mStringBuffer;
    eRequestLine mRequestLine;

  public:
    ReadEvent(const Server &server, int clientSocket);
    virtual ~ReadEvent();
    virtual int handle();
};

#endif