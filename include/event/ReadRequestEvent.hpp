#ifndef READREQUESTEVENT_HPP
#define READREQUESTEVENT_HPP

#include "AEvent.hpp"

class ReadRequestEvent : public AEvent
{
  private:
    std::string mStringBuffer;

  public:
    ReadRequestEvent(const Server &server, int clientSocket);
    virtual ~ReadRequestEvent();
    virtual int handle();
    void makeReadFileEvent();
};

#endif