#ifndef AEVENT_HPP
#define AEVENT_HPP

#include "Server.hpp"

class AEvent
{
  private:
    // response
    // request
    const Server &mServer;

  public:
    AEvent(const Server &server);
    virtual void handle() = 0;
};

#endif