#ifndef KEYBOARDOUTPUT_H
#define KEYBOARDOUTPUT_H

#include <list>

#include "IOutput.h"
#include "Keyboard.h"

namespace eyeoh {

	class KeyboardOutput : public IOutput
	{
	public:
		static IOutput * Create(const Keyboard & keyboard, const std::string & value, const std::list<Key> & triggerKeys);

	private:
		const Keyboard * keyboard;
		std::list<Key> * keys;

	public:
		KeyboardOutput(const Keyboard & keyboard, std::list<Key> & keys);
		virtual ~KeyboardOutput();

		virtual void Do();
	};

}

#endif