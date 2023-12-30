#ifndef UTIL_HPP
#define UTIL_HPP

#include <fstream>
#include <string>

bool isWhiteSpace(const char c);
std::string trim(const std::string &str);
void trimLine(std::string &line);
void deleteComments(std::string &line);
void ftGetLine(std::ifstream &file, std::string &line);

#endif