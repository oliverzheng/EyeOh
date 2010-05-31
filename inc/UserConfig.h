#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <string>
#include <map>
#include <vector>

#include "StateMap.h"
#include "Keyboard.h"

namespace eyeoh {
	typedef IOutput * (* OutputFactoryFunc)(const Keyboard &, const std::string &);

	class UserConfig
	{
	private:
		static const char special_open = '{';
		static const char special_close = '}';

		Keyboard & keyboard;
		StateMap statemap;
		std::vector<OutputFactoryFunc> outputFactoryFuncs;

	public:
		UserConfig(Keyboard & keyboard);

		void AddOutput(OutputFactoryFunc outputFactoryFunc);

		bool ParseFile(std::string filepath);

		StateMap & GetStateMap();
	};

}

#endif