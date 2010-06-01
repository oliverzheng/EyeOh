#ifndef MODESWITCHER_H
#define MODESWITCHER_H

#include "StateMap.h"

namespace eyeoh {
	class ModeSwitcher : public IOutput
	{
	public:
		enum Mode
		{
			COMMAND_MODE,
			PASSTHROUGH_MODE,
		};

	private:
		static const char * const COMMAND_NAME;
		static const char * const PASSTHROUGH_NAME;
		static StateMap * statemap;
		static Key enableKey;

	public:
		static void SetStateMap(StateMap & statemap);
		static IOutput * Create(const Keyboard & keyboard, const std::string & value, const std::list<Key> & triggerKeys);

	private:
		Mode mode;

		ModeSwitcher(Mode mode);

	public:
		virtual ~ModeSwitcher();

		virtual void Do();
	};
}

#endif