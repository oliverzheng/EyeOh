#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include <vector>
#include <stdexcept>
#include <list>

namespace eyeoh {

	typedef std::string KeyName;

	class KeyConflict : public std::logic_error
	{
	public:
		KeyConflict();
	};

	class NoSuchKey : public std::domain_error
	{
	public:
		NoSuchKey();
	};

	struct Key
	{
		enum Modifiers
		{
			NONE	= 0x00,
			SHIFT	= 0x01,
			CTRL	= 0x02,
			ALT		= 0x04,
		};

		unsigned short key;
		unsigned short modifiers;

		Key() : key(0), modifiers(0) {};

		// Required for use as an indexing type in std::map
		bool operator<(const Key & key) const;
	};

	class Keyboard
	{
	public:
		static const char MACRO_OPEN = '{';
		static const char MACRO_CLOSE = '}';

		virtual const std::vector<KeyName> & GetKeyNames() const = 0;
		virtual Key operator[](KeyName & keyname) const = 0;
		virtual KeyName operator[](Key & keypress) const = 0;
		virtual bool contains(KeyName & keyname) const = 0;
		virtual bool contains(Key & keypress) const = 0;
		virtual const Keyboard & operator<<(Key & keypress) const = 0;

		// If keynames match, fills in the list of keys and returns true.
		// Returns false otherwise. This includes modifier keys, thus can have spaces.
		bool GetKeys(const KeyName & value, std::list<Key> & keys) const;

		// If the keyname matches a key, fills in key and returns true.
		// Returns false otherwise.
		bool GetSequenceKeys(const KeyName & keynames, std::list<Key> & keys) const;

		// If the keyname matches a key, fills in key and returns true.
		// Returns false otherwise.
		bool GetModifierKey(const KeyName & keyname, Key & key) const;
	};

}

#endif