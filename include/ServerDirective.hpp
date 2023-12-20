#ifndef SERVERDIRECTIVE_HPP
#define SERVERDIRECTIVE_HPP

#include "ADirective.hpp"

class ServerDirective : public ADirective
{
  protected:
    virtual void initDirectiveMap();

  public:
    ServerDirective();
};
#endif