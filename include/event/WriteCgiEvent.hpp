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
    int mSocket;

  public:
    WriteCgiEvent(const Server &server, int clientSocket, int socket, std::string message);
    virtual ~WriteCgiEvent();
    virtual void handle();
};

#endif