#ifndef IOUTPUT_H
#define IOUTPUT_H

namespace eyeoh {

	class IOutput
	{
	public:
		virtual void Do() = 0;
		virtual ~IOutput() {};
	};

}

#endif