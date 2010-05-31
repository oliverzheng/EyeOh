#include <sstream>

#include "KeyboardOutput.h"

using namespace eyeoh;
using namespace std;

IOutput * KeyboardOutput::Create(const Keyboard & keyboard, const string & value)
{
	list<Key> & keys = *new list<Key>;

	for (string::const_iterator it = value.begin(); it != value.end(); it++)
	{
		string keyname;
		stringstream ss;
		ss << *it;
		ss >> keyname;

		keys.push_back(keyboard[keyname]);
	}
	return new KeyboardOutput(keyboard, keys);
}

KeyboardOutput::KeyboardOutput(const Keyboard & keyboard, list<Key> & keys)
	: keyboard(keyboard), keys(keys)
{
}

void KeyboardOutput::Do()
{
	for (list<Key>::iterator it = this->keys.begin(); it != this->keys.end(); it++)
	{
		this->keyboard << *it;
	}
}
