#ifndef UTIL_HPP
#define UTIL_HPP

#include <fstream>
#include <string>

bool isWhiteSpace(const char c);
void trimLine(std::string &line);
void deleteComments(std::string &line);
void ftGetLine(std::ifstream &file, std::string &line);

#endif