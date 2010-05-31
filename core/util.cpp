#include <sstream>

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