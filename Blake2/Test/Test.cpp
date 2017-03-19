#include <sstream> 
#include "Blake2Test.h"
#include "DigestSpeedTest.h"
#include "ConsoleUtils.h"
#include "HexConverter.h"
#include "ITest.h"

using namespace TestBlake2;

std::string GetResponse()
{
	std::string resp;
	std::getline(std::cin, resp);

	return resp;
}

int StringToInt(const std::string Text)
{
	std::istringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
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
	RunTest(new Blake2Test());

	try
	{
		PrintHeader("Warning! Compile as Release with correct platform (x86/x64) for accurate timings");
		PrintHeader("", "");

		if (CanTest("Press 'Y' then Enter to run Diagnostic Tests, any other key to cancel: "))
		{
			RunTest(new Blake2Test());
		}
		else
		{
			ConsoleUtils::WriteLine("Diagnostic test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");

		if (CanTest("Press 'Y' then Enter to run Message Digest Speed Tests, any other key to cancel: "))
		{
			RunTest(new DigestSpeedTest(0));
		}
		else
		{
			ConsoleUtils::WriteLine("Speed test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");


		if (CanTest("Press 'Y' then Enter to run extended C/C++ version comparison test on 20GB, any other key to cancel: "))
		{
			RunTest(new DigestSpeedTest(1));
		}
		else
		{
			ConsoleUtils::WriteLine("Extended Speed test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");

		if (CanTest("Press 'Y' then Enter to run user defined Parallel Degree Test, any other key to cancel: "))
		{
			PrintHeader("Enter the Parallel Degree: must be greater than and divisible by 4 (4, 8, 12, 16, 20, 24, 28, 32");
			int thds = 0;
			do
			{
				std::string resp;
				std::getline(std::cin, resp);
				thds = StringToInt(resp);
				if (thds < 4 || thds % 4 != 0 || thds > 32)
				{
					thds = 0;
					PrintHeader("Enter the Parallel Degree: must be greater than and divisible by 4 (ex 4,8,12,16..");
				}
			} 
			while (thds == 0);

			RunTest(new DigestSpeedTest(thds));
		}
		else
		{
			ConsoleUtils::WriteLine("Custom Parallel Degree test was Cancelled..");
		}
		ConsoleUtils::WriteLine("");


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