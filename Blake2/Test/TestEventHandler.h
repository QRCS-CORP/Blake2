#ifndef _BLAKE2TEST_TESTEVENTHANDLER_H
#define _BLAKE2TEST_TESTEVENTHANDLER_H

#include "TestEvent.h"
#include "ConsoleUtils.h"

namespace BlakeTest
{
	class TestEventHandler : public TestEvent<TestEventHandler>
	{
	public:
		void operator()(const char* Data)
		{
			ConsoleUtils::WriteLine(std::string(Data));
		}
	};
}

#endif