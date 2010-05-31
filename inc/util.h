#ifndef UTIL_H
#define UTIL_H

#include <list>
#include <string>
#include <istream>

#include "UserConfig.h"
#include "IOutput.h"

std::list<std::string> split(const std::string &s, char delim);
std::istream & getliner(std::istream & file, std::string & line);
void strip(std::string & line);

#endif