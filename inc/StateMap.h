#ifndef STATEMAP_H
#define STATEMAP_H

#include <list>
#include <vector>
#include <map>
#include <utility>

#include "Keyboard.h"
#include "IOutput.h"

namespace eyeoh {

	class NoOutput : public std::logic_error
	{
	public:
		NoOutput();
	};

	class StateMap
	{
	public:
		enum ChangeResult
		{
			CHANGED,
			RESET,
		};

		enum OutputResult
		{
			NO_OUTPUT,
			OUTPUT,
		};

	private:
		StateMap * root;
		StateMap * current;
		std::map<Key, StateMap *> keys;
		IOutput * output;

	public:
		StateMap();

		// Build states
		StateMap & operator<<(Key & key);
		StateMap & operator<<(IOutput & output);

		// Query current state
		bool HasOutput() const;
		IOutput & GetOutput() const;
		bool IsEnd() const;
		StateMap & operator[](Key & key) const;

		// Change the state of the current state machine
		std::pair<ChangeResult, OutputResult> OnPress(Key & key);
		void Reset();
	};

}

#endif