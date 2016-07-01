#include "Blake2Test.h"
#include "DigestSpeedTest.h"
#include "ConsoleUtils.h"
#include "HexConverter.h"
#include "ITest.h"

using namespace BlakeTest;

std::string GetResponse()
{
	std::string resp;
	std::getline(std::cin, resp);

	return resp;
}

bool CanTest(std::string Message)
{
	ConsoleUtils::WriteLine(Message);
	std::string resp = GetResponse();
	std::transform(resp.begin(), resp.end(), resp.begin(), ::toupper);

	const std::string CONFIRM = "Y";
	if (resp.find(CONFIRM) != std::string::npos)
		return true;

	return false;
}

void PrintHeader(std::string Data, std::string Decoration = "***")
{
	ConsoleUtils::WriteLine(Decoration + Data + Decoration);
}

void PrintTitle()
{
	ConsoleUtils::WriteLine("**********************************************");
	ConsoleUtils::WriteLine("* Blake2++ Version 1.0: The CEX Library      *");
	ConsoleUtils::WriteLine("*                                            *");
	ConsoleUtils::WriteLine("* Release:   v1.0                            *");
	ConsoleUtils::WriteLine("* Date:      June 19, 2016                   *");
	ConsoleUtils::WriteLine("* Contact:   develop@vtdev.com               *");
	ConsoleUtils::WriteLine("**********************************************");
	ConsoleUtils::WriteLine("");
}

void CloseApp()
{
	PrintHeader("An error has occurred! Press any key to close..", "");
	GetResponse();
	exit(0);
}

void RunTest(Blake2Test::ITest* Test)
{
	try
	{
		TestEventHandler handler;
		Test->Progress() += &handler;
		ConsoleUtils::WriteLine(Test->Description());
		ConsoleUtils::WriteLine(Test->Run());
		Test->Progress() -= &handler;
		ConsoleUtils::WriteLine("");

		delete Test;
	}
	catch (TestException &ex)
	{
		ConsoleUtils::WriteLine("An error has occured!");

		if (ex.Message().size() != 0)
			ConsoleUtils::WriteLine(ex.Message());

		ConsoleUtils::WriteLine("");
		ConsoleUtils::WriteLine("Continue Testing? Press 'Y' to continue, all other keys abort..");

		std::string resp;
		std::getline(std::cin, resp);
		std::transform(resp.begin(), resp.end(), resp.begin(), ::toupper);

		const std::string CONTINUE = "Y";
		if (resp.find(CONTINUE) == std::string::npos)
			CloseApp();
	}
}

int main()
{
	ConsoleUtils::SizeConsole();
	PrintTitle();

	try
	{
		PrintHeader("Warning! Compile as Release with correct platform (x86/x64) for accurate timings");
		PrintHeader("", "");

		if (CanTest("Press 'Y' then Enter to run Message Digest Speed Tests, any other key to cancel: "))
		{
			// unrem to get better avg. sampling
			//for(size_t i = 0; i < 10; ++i)
			RunTest(new DigestSpeedTest());
		}
		else
		{
			ConsoleUtils::WriteLine("Speed test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");


		if (CanTest("Press 'Y' then Enter to run a C/C++ version comparison test on 100GB, any other key to cancel: "))
		{
			RunTest(new DigestSpeedTest(true));
		}
		else
		{
			ConsoleUtils::WriteLine("Speed comparison test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");


		if (!CanTest("Press 'Y' then Enter to run Diagnostic Tests, any other key to cancel: "))
		{
			ConsoleUtils::WriteLine("Completed! Press any key to close..");
			GetResponse();
			return 0;
		}
		ConsoleUtils::WriteLine("");

		PrintHeader("TESTING CRYPTOGRAPHIC HASH GENERATORS");
		RunTest(new Blake2Test());


		PrintHeader("Completed! Press any key to close..", "");
		GetResponse();

		return 0;
	}
	catch (...)
	{
		PrintHeader("An error has occurred! Press any key to close..", "");
		GetResponse();

		return 0;
	}
}