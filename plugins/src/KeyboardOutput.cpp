#include <sstream>

#include "KeyboardOutput.h"

using namespace eyeoh;
using namespace std;

IOutput * KeyboardOutput::Create(const Keyboard & keyboard, const string & value)
{
	list<Key> & keys = *new list<Key>;

	if (!keyboard.GetKeys(value, keys))
	{
		delete &keys;
		return NULL;
	}

	return new KeyboardOutput(keyboard, keys);
}

KeyboardOutput::KeyboardOutput(const Keyboard & keyboard, list<Key> & keys)
	: keyboard(keyboard), keys(keys)
{
}

KeyboardOutput::~KeyboardOutput()
{
	delete &(this->keys);
}

void KeyboardOutput::Do()
{
	for (list<Key>::iterator it = this->keys.begin(); it != this->keys.end(); it++)
	{
		this->keyboard << *it;
	}
}