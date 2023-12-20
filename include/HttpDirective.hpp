#ifndef HTTPDIRECTIVE_HPP
#define HTTPDIRECTIVE_HPP

#include "ADirective.hpp"

class HttpDirective : public ADirective
{
  protected:
    virtual void initDirectiveMap();

  public:
    HttpDirective();
};

#endif