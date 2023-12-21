#ifndef SERVERDIRECTIVE_HPP
#define SERVERDIRECTIVE_HPP

#include "ADirective.hpp"

class ServerDirective : public ADirective
{
  public:
    ServerDirective();
    virtual void initDirectiveMap();
};
#endif