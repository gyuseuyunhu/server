#ifndef LOCATIONDIRECVITE_HPP
#define LOCATIONDIRECVITE_HPP

#include "ADirective.hpp"

class LocationDirective : public ADirective
{
  public:
    LocationDirective();
    virtual void initDirectiveMap();
};
#endif