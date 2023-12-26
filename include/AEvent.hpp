#ifndef AEVENT_HPP
#define AEVENT_HPP

#include "Server.hpp"

class AEvent
{
  protected:
    // response
    // request
    const Server &mServer;

  public:
    AEvent(const Server &server);
    virtual ~AEvent();
    virtual void handle() = 0;
};

#endif