#include <sstream>
#include <list>
#include <algorithm>

#include "util.h"
#include "Keyboard.h"

using namespace eyeoh;
using namespace std;

KeyConflict::KeyConflict()
	: logic_error("A keycode is used for more than 1 keyname")
{
}

NoSuchKey::NoSuchKey()
	: domain_error("No such key exists.")
{
}

bool Key::operator<(const Key & key) const
{
	if (this->key < key.key)
		return true;
	else if (this->key == key.key)
		return (this->modifiers < key.modifiers);
	else
		return false;
}

bool Keyboard::GetKeys(const KeyName & value, list<Key> & keys) const
{
	list<string> values = split(value, ' ');
	for (list<string>::iterator it = values.begin(); it != values.end(); it++)
	{
		Key key;
		if (this->GetModifierKey(*it, key))
		{
			keys.push_back(key);
			continue;
		}

		list<Key> it_keys;
		if (this->GetSequenceKeys(*it, it_keys))
		{
			keys.splice(keys.end(), it_keys);
			continue;
		}
		return false;
	}
	return true;
}

bool Keyboard::GetSequenceKeys(const KeyName & keynames, list<Key> & keys) const
{
	for (KeyName::const_iterator it = keynames.begin(); it != keynames.end(); it++)
	{
		string name;
		if (*it == Keyboard::MACRO_OPEN)
		{
			KeyName::const_iterator close_end = find(it, keynames.end(), Keyboard::MACRO_CLOSE);
			if (close_end == keynames.end())
				return false;
			it++;
			name = string(it, close_end);
			it = close_end;
		}
		else
		{
			name = string(1, *it);
		}

		if (!this->contains(name))
			return false;

		keys.push_back((*this)[name]);
	}
	return true;
}

bool Keyboard::GetModifierKey(const KeyName & keyname, Key & key) const
{
	list<string> keynames = split(keyname, '-');
	if (keynames.size() != 2)
		return false;

	string modifiers = keynames.front();

	// I don't like how the keynames are hardcoded in here. TODO.
	for (KeyName::const_iterator it = modifiers.begin(); it != modifiers.end(); it++)
	{
		switch (*it)
		{
		case 'C':
			key.modifiers |= Key::CTRL;
			break;

		case 'A':
			key.modifiers |= Key::ALT;
			break;

		case 'S':
			key.modifiers |= Key::SHIFT;
			break;

		default:
			return false;
		}
	}

	string key_single = keynames.back();
	list<Key> keys;
	if (!this->GetSequenceKeys(key_single, keys))
		return false;

	if (keys.size() != 1)
		return false;

	key.key = keys.front().key;

	return true;
}