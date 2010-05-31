#ifndef WINDOWSKEYBOARD_H
#define WINDOWSKEYBOARD_H

#include <map>

#include "Keyboard.h"

namespace eyeoh {
	class WindowsKeyboard : public Keyboard
	{
	private:
		std::vector<KeyName> names;
		std::map<KeyName, Key> namesToKeys;
		std::map<Key, KeyName> keysToNames;

	public:
		WindowsKeyboard(const char * keys);
		
		virtual const std::vector<KeyName> & GetKeyNames() const;
		virtual Key operator[](KeyName & keyname) const;
		virtual KeyName operator[](Key & keypress) const;
		virtual bool contains(KeyName & keyname) const;
		virtual bool contains(Key & key) const;
		virtual const Keyboard & operator<<(Key & key)  const;
	};
}

#endif