#ifndef UTIL_HPP
#define UTIL_HPP

#include <fstream>
#include <string>

void trimLine(std::string &line);
void deleteComments(std::string &line);
void ftGetLine(std::ifstream &file, std::string &line);

#endif