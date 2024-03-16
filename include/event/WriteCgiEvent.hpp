#ifndef WRITECGIEVENT_HPP
#define WRITECGIEVENT_HPP

#include "AEvent.hpp"
#include <ctime>
#include <unistd.h>

class WriteCgiEvent : public AEvent
{
  private:
    std::string mMessage;
    int mWriteSize;
    int mFileSize;
    int mFd;

  public:
    WriteCgiEvent(const Server &server, int clientSocket, int fd, std::string message);
    virtual ~WriteCgiEvent();
    virtual void handle();
    virtual void timer();
    virtual int getFd() const;
};

#endif