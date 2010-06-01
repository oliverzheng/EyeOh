#include "keyboardhook.h"

#include "ModeSwitcher.h"

using namespace eyeoh;

const char * const ModeSwitcher::COMMAND_NAME = "Reset()";
const char * const ModeSwitcher::PASSTHROUGH_NAME = "PassThrough()";

StateMap * ModeSwitcher::statemap = NULL;
Key ModeSwitcher::enableKey = Key();

void ModeSwitcher::SetStateMap(StateMap & statemap)
{
	ModeSwitcher::statemap = &statemap;
}

IOutput * ModeSwitcher::Create(const Keyboard & keyboard, const std::string & value, const std::list<Key> & triggerKeys)
{
	// Currently only supports a single key to trigger command mode. :(
	// Need some sort of incoming key filtering system to have multi-key triggers.
	if (value.compare(ModeSwitcher::COMMAND_NAME) == 0)
	{
		ModeSwitcher::enableKey = triggerKeys.back();
		return new ModeSwitcher(ModeSwitcher::COMMAND_MODE);
	}

	if (value.compare(ModeSwitcher::PASSTHROUGH_NAME) == 0)
		return new ModeSwitcher(ModeSwitcher::PASSTHROUGH_MODE);

	return NULL;
}

ModeSwitcher::ModeSwitcher(Mode mode)
	: mode(mode)
{
}

ModeSwitcher::~ModeSwitcher()
{
}

void ModeSwitcher::Do()
{
	if (this->mode == ModeSwitcher::COMMAND_MODE)
	{
		if (this->statemap != NULL)
			statemap->Reset();
		Enable();
	}
	else if (this->mode == ModeSwitcher::PASSTHROUGH_MODE)
	{
		Disable(this->enableKey.key);
	}
}