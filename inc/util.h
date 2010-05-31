#ifndef UTIL_H
#define UTIL_H

#include <list>
#include <string>

#include "UserConfig.h"
#include "IOutput.h"

std::list<std::string> split(const std::string &s, char delim);

#endif