#ifndef WRITEEVENT_HPP
#define WRITEEVENT_HPP

#include "AEvent.hpp"
#include <unistd.h>

class WriteEvent : public AEvent
{
  private:
    std::string mMessage;
    int mWriteSize;
    int mResponseSize;

  public:
    WriteEvent(const Server &server, const Response &response, int clientSocket);
    virtual ~WriteEvent();
    virtual void handle();
    virtual int getFd() const;
};

#endif