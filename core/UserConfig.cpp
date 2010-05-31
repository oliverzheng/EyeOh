#include <fstream>
#include <sstream>

#include "UserConfig.h"

using namespace std;
using namespace eyeoh;

UserConfig::UserConfig(Keyboard & keyboard)
	: keyboard(keyboard)
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

		string input, value;
		while(!file.eof())
		{
			// Input is the user keystrokes; value is the expanded keystrokes.
			file >> input >> value;
			StateMap * statemap = &(this->statemap);

			// For each keystroke, extend the transition state machine by a new state.
			for (string::iterator it = input.begin(); it != input.end(); it++)
			{
				string keyname;
				if (*it == UserConfig::special_open)
				{
					continue;
				}
				else
				{
					stringstream ss;
					ss << *it;
					ss >> keyname;
				}

				statemap = &((*statemap) << this->keyboard[keyname]);
			}

			IOutput * output = NULL;
			for (vector<OutputFactoryFunc>::iterator it = this->outputFactoryFuncs.begin();
				it != this->outputFactoryFuncs.end() && output == NULL; it++)
			{
				output = (**it)(this->keyboard, value);
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