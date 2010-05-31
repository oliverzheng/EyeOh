#include <fstream>
#include <sstream>

#include <Windows.h>

#include "keyboardhook.h"

#include "util.h"
#include "WindowsKeyboard.h"

using namespace eyeoh;
using namespace std;

WindowsKeyboard::WindowsKeyboard(const char * keys)
{
	istringstream file(keys);

	while(file && !file.eof())
	{
		KeyName name;

		// Get key name
		getline(file, name, '\t');
		if (name.empty())
		{
			// Empty line at the end of the file
			break;
		}

		Key keypress;
		string key_s;
		list<string> key_l;

		// Extract hex keycode
		getliner(file, key_s);

		key_l = split(key_s, ' ');
		string keycode_s = key_l.front();
		istringstream ss(keycode_s);
		ss >> hex >> keypress.key;
		key_l.pop_front();

		// Get key modifiers
		for (list<string>::iterator it = key_l.begin(); it != key_l.end(); it++)
		{
			if (it->compare("S") == 0)
				keypress.modifiers |= Key::SHIFT;

			if (it->compare("C") == 0)
				keypress.modifiers |= Key::CTRL;

			if (it->compare("A") == 0)
				keypress.modifiers |= Key::ALT;
		}

		this->namesToKeys[name] = keypress;
	}

	// Populate key->name map and names list.
	for (map<KeyName, Key>::const_iterator it = this->namesToKeys.begin();
			it != this->namesToKeys.end(); it++)
	{
		if (this->keysToNames.find(it->second) != this->keysToNames.end())
		{
			throw KeyConflict();
		}
		this->keysToNames[it->second] = it->first;
		this->names.push_back(it->first);
	}
}

const vector<KeyName> & WindowsKeyboard::GetKeyNames() const
{
	return this->names;
}

Key WindowsKeyboard::operator[](KeyName & keyname) const
{
	map<KeyName, Key>::const_iterator it = this->namesToKeys.find(keyname);
	if (it != this->namesToKeys.end())
	{
		return it->second;
	}
	else
	{
		throw NoSuchKey();
	}
}

KeyName WindowsKeyboard::operator[](Key & keypress) const
{
	map<Key, KeyName>::const_iterator it = this->keysToNames.find(keypress);
	if (it != this->keysToNames.end())
	{
		return it->second;
	}
	else
	{
		throw NoSuchKey();
	}
}

bool WindowsKeyboard::contains(KeyName & keyname) const
{
	return this->namesToKeys.find(keyname) != this->namesToKeys.end();
}

bool WindowsKeyboard::contains(Key & keypress) const
{
	return this->keysToNames.find(keypress) != this->keysToNames.end();
}

const Keyboard & WindowsKeyboard::operator<<(Key & keypress) const
{
	InjectKey(keypress);
	return *this;
}