#ifndef ACCEPTEVENT_HPP
#define ACCEPTEVENT_HPP

#include "AEvent.hpp"

class AcceptEvent : public AEvent
{
  public:
    AcceptEvent(const Server &server);
    ~AcceptEvent();
    virtual void handle();
    virtual int getFd() const;
};

#endif