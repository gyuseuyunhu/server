#ifndef ACCEPT_HPP
#define ACCEPT_HPP

#include "AEvent.hpp"

class Accept : public AEvent
{
  public:
    Accept(const Server &server);
    ~Accept();
    void handle();
};

#endif