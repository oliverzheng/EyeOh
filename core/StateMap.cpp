#include "StateMap.h"

using namespace eyeoh;
using namespace std;

NoOutput::NoOutput()
	: logic_error("No output available for this state")
{
}


StateMap::StateMap()
	: root(NULL), current(NULL), output(NULL)
{
	root = this;
	current = this;
}

StateMap & StateMap::operator<<(Key & key)
{
	map<Key, StateMap *>::iterator it = this->keys.find(key);
	if (it != this->keys.end())
	{
		return *(it->second);
	}
	else
	{
		StateMap * statemap = new StateMap();
		this->keys[key] = statemap;
		return *statemap;
	}
}

StateMap & StateMap::operator<<(IOutput & output)
{
	this->current->output = &output;
	return *(this->current);
}

bool StateMap::HasOutput() const
{
	return this->current->output != NULL;
}

IOutput & StateMap::GetOutput() const
{
	if (this->current->output != NULL)
		return *(this->current->output);
	else
		throw NoOutput();
}

bool StateMap::IsEnd() const
{
	return this->current->keys.empty();
}

StateMap & StateMap::operator[](Key & key) const
{
	map<Key, StateMap *>::const_iterator it = this->keys.find(key);
	if (it != this->keys.end())
		return *(it->second);
	else
		throw NoSuchKey();
}

pair<StateMap::ChangeResult, StateMap::OutputResult> StateMap::OnPress(Key & key)
{
	pair<StateMap::ChangeResult, StateMap::OutputResult> result;

	map<Key, StateMap *>::iterator it = this->current->keys.find(key);
	if (it != this->current->keys.end())
	{
		this->current = it->second;
		if (this->HasOutput())
		{
			result.second = StateMap::OUTPUT;
			this->GetOutput().Do();
		}
		else
		{
			result.second = StateMap::NO_OUTPUT;
		}

		if (this->current->keys.empty())
		{
			this->current = this->root;
			result.first = StateMap::RESET;
		}
		else
		{
			result.first = StateMap::CHANGED;
		}
	}
	else
	{
		this->current = this->root;
		result.first = StateMap::RESET;
		result.second = StateMap::NO_OUTPUT;
	}

	return result;
}

void StateMap::Reset()
{
	this->current = this->root;
}