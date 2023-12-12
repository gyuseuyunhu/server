#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>

class Config
{
  private:
    int mServerNum; // 서버 블록 개수

  public:
    Config(std::string &path);
};

#endif