#ifndef AEVENT_HPP
#define AEVENT_HPP

#include "Request.hpp"
#include "Server.hpp"

#define EVENT_FINISH 1
#define EVENT_CONTINUE 0

class AEvent
{
  protected:
    // response
    Request mRequest;
    const Server &mServer;

  public:
    AEvent(const Server &server);
    virtual ~AEvent();
    virtual int handle() = 0;
};

#endif