#include <sstream>
#include <algorithm>

#include "util.h"

using namespace eyeoh;
using namespace std;

list<string> split(const string &s, char delim) {
    list<string> elems;
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

istream & getliner(istream & file, string & line)
{
	istream & ret = getline(file, line);

	// Remove \r at the end of the line.
	size_t slash_r = line.find_last_of('\r');
	if (slash_r != string::npos)
		line.erase(slash_r);

	return ret;
}

void strip(string & line)
{
	line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
}