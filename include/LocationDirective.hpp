#ifndef LOCATIONDIRECVITE_HPP
#define LOCATIONDIRECVITE_HPP

#include "ADirective.hpp"

class LocationDirective : public ADirective
{
  protected:
    virtual void initDirectiveMap();

  public:
    LocationDirective();
};
#endif