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