#ifndef _BLAKE2TEST_DIGESTSPEEDTEST_H
#define _BLAKE2TEST_DIGESTSPEEDTEST_H

#include "ITest.h"
#include "../Blake2/Digests.h"

namespace TestBlake2
{
	using CEX::Enumeration::Digests;

	/// <summary>
	/// Blake2 Digest Speed Tests
	/// </summary>
	class DigestSpeedTest : public ITest
	{
	private:
		const std::string DESCRIPTION = "Digest Speed Tests.";
		const std::string FAILURE = "FAILURE! ";
		const std::string MESSAGE = "COMPLETE! Speed tests have executed succesfully.";
		static constexpr uint64_t KB1 = 1000;
		static constexpr uint64_t MB1 = KB1 * 1000;
		static constexpr uint64_t MB10 = MB1 * 10;
		static constexpr uint64_t MB100 = MB1 * 100;
		static constexpr uint64_t GB1 = MB1 * 1000;
		static constexpr uint64_t GB10 = GB1 * 10;
		static constexpr uint64_t DEFITER = 10;

		int m_testCycle;
		TestEventHandler m_progressEvent;

	public:
		/// <summary>
		/// Get: The test description
		/// </summary>
		virtual const std::string Description() { return DESCRIPTION; }

		/// <summary>
		/// Progress return event callback
		/// </summary>
		virtual TestEventHandler &Progress() { return m_progressEvent; }

		/// <summary>
		/// Test Blake2 for performance
		/// </summary>
		///
		/// <param name="TestCycle">The type of speed test to run; standard(0), long(1), or extended parallel degree (4 or greater, must be divisible by 4)</param>
		DigestSpeedTest(int TestCycle = 0)
			:
			m_testCycle(TestCycle)
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run()
		{
			try
			{
				OnProgress("***The sequential Blake2 256 digest***");
				DigestBlockLoop(Digests::Blake256, MB100, 10, false);
				OnProgress("***The parallel Blake2 256 digest***");
				DigestBlockLoop(Digests::Blake256, MB100, 10, true);
				OnProgress("***The sequential Blake2 512 digest***");
				DigestBlockLoop(Digests::Blake512, MB100, 10, false);
				OnProgress("***The parallel Blake2 512 digest***");
				DigestBlockLoop(Digests::Blake512, MB100, 10, true);

				return MESSAGE;
			}
			catch (std::string &ex)
			{
				return FAILURE + " : " + ex;
			}
			catch (...)
			{
				return FAILURE + " : Internal Error";
			}
		}

	private:

		void DigestBlockLoop(Digests DigestType, size_t SampleSize, size_t Loops = DEFITER, bool Parallel = false);
		uint64_t GetBytesPerSecond(uint64_t DurationTicks, uint64_t DataSize);
		void OnProgress(char* Data);
	};
}

#endif