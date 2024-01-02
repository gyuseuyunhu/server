#ifndef ADIRECTIVE_HPP
#define ADIRECTIVE_HPP

#include <map>
#include <string>
class ADirective
{
  protected:
    std::map<std::string, int> mDirective;
    virtual void initDirectiveMap() = 0;

  public:
    void checkDirective(const std::string &str);
};

#endif