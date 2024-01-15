#ifndef UTIL_HPP
#define UTIL_HPP

#include <fcntl.h>
#include <fstream>
#include <string>

int nonBlockOpen(const char *str, int flag);
bool isWhiteSpace(const char c);
std::string trim(const std::string &str);
void trimLine(std::string &line);
void deleteComments(std::string &line);
void ftGetLine(std::ifstream &file, std::string &line);
bool caseInsensitiveMatch(const std::string &str1, const std::string &str2);

#endif