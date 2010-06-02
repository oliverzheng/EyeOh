#include <fstream>
#include <sstream>

#include "util.h"
#include "UserConfig.h"

using namespace std;
using namespace eyeoh;

UserConfig::UserConfig(Keyboard & keyboard)
	: keyboard(&keyboard)
{
}

void UserConfig::AddOutput(OutputFactoryFunc outputFactoryFunc)
{
	this->outputFactoryFuncs.push_back(outputFactoryFunc);
}

bool UserConfig::ParseFile(string filepath)
{
	ifstream file(filepath);
	if (file.is_open())
	{
		file >> skipws;

		string input;
		while(!file.eof())
		{
			string line;

			// Input is the user keystrokes
			getliner(file, line);

			// Blank lines
			if (line.empty())
				continue;

			list<string> io = split(line, '=');
			if (io.size() != 2)
				throw logic_error("There isn't exactly 1 '=' in the user config");
			input = io.front();
			strip(input);

			StateMap * statemap = &(this->statemap);

			// For each keystroke, extend the transition state machine by a new state.
			list<Key> keys;
			if (!this->keyboard->GetKeys(input, keys))
				return false;
			for (list<Key>::iterator it = keys.begin(); it != keys.end(); it++)
				statemap = &((*statemap) << *it);

			string value = io.back();
			strip(value);
			IOutput * output = NULL;
			for (vector<OutputFactoryFunc>::iterator it = this->outputFactoryFuncs.begin();
				it != this->outputFactoryFuncs.end() && output == NULL; it++)
			{
				output = (**it)(*(this->keyboard), value, keys);
			}

			(*statemap) << *output;
		}

		file.close();

		return true;
	}
	else
	{
		return false;
	}
}

StateMap & UserConfig::GetStateMap()
{
	return this->statemap;
}