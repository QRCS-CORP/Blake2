#ifndef _BLAKE2TEST_DIGESTSPEEDTEST_H
#define _BLAKE2TEST_DIGESTSPEEDTEST_H

#include "ITest.h"
#include <sstream>

namespace BlakeTest
{
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
				if (m_testCycle == 0)
				{
					OnProgress("*** TEST PARAMETERS ***");
					OnProgress("Blake2B, Blake2BP, Blake2S, and Blake2SP are all tested for performance.");
					OnProgress("Speed is measured in MegaBytes (1,000,000 bytes) per Second, with a sample size of 20 GB.");
					OnProgress("20 * 1GB loops are run and added for the combined average over 20 GigaBytes of data.");
					OnProgress("Block update sizes are fixed at 250MB times 4 iterations per 1GB loop cycle.");
					OnProgress("The first run uses the optimized C version to establish a baseline for each of the algorithms.");
					OnProgress("The second run uses the CEX C++ version of the algorithms.");
					OnProgress("Both the optimized C and C++ versions use identical parameter sets.");
					OnProgress("");

					OnProgress("### Original C version BLAKE2BP Message Digest: 20 loops * 1000 MB ###");
					CBlake2BPLoop(GB1, 20);

					OnProgress("### CEX C++ BLAKE2BP Message Digest: 20 loops * 1000 MB ###");
					CppBlake2BLoop(GB1, 20, true);
				}
				else if (m_testCycle == 1)
				{
					OnProgress("*** TEST PARAMETERS ***");
					OnProgress("Blake2B, Blake2BP, Blake2S, and Blake2SP are all tested for performance.");
					OnProgress("Speed is measured in MegaBytes (1,000,000 bytes) per Second, with a sample size of 1 GB.");
					OnProgress("Block update sizes are fixed at 250MB times 4 iterations per 1GB loop cycle.");
					OnProgress("10 * 1GB loops are run and added for the combined average over 10 GigaByte of data.");
					OnProgress("The first run uses the optimized C version to establish a baseline for each of the algorithms.");
					OnProgress("The second run uses the CEX C++ version of the algorithms.");
					OnProgress("Both the optimized C and C++ versions use identical parameter sets.");
					OnProgress("");

					OnProgress("### Blake2B Optimized C Sequential: 10 loops * 1000 MB ###");
					CBlake2BLoop(GB1);
					OnProgress("### Blake2BP Optimized C Parallel: 10 loops * 1000 MB ###");
					CBlake2BPLoop(GB1);

					OnProgress("### Blake2B-512 C++ Sequential: 10 loops * 1000 MB ###");
					CppBlake2BLoop(GB1, 10);
					OnProgress("### Blake2BP-512 C++ Parallel: 10 loops * 1000 MB ###");
					CppBlake2BLoop(GB1, 10, true);
				}
				else
				{
					OnProgress("*** TEST PARAMETERS ***");
					OnProgress("Blake2B and Blake2BP are all tested for performance using a user defined Parallel Degree.");
					OnProgress("Speed is measured in MegaBytes (1,000,000 bytes) per Second, with a sample size of 20 GB.");
					OnProgress("20 * 1GB loops are run and added for the combined average over 20 GigaBytes of data.");
					OnProgress("Block update sizes are fixed at sample size divided by the requested Thread per each 1GB loop cycle.");
					OnProgress("The second run uses the CEX C++ version of the algorithms.");
					OnProgress("Both the optimized C and C++ versions use identical parameter sets.");
					OnProgress("");

					OnProgress("### Blake2B-512 C++ Sequential: 10 loops * 1000 MB ###");
					CppBlake2BEx(GB1, m_testCycle, 20);
				}

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
		void CBlake2BLoop(size_t SampleSize, size_t Loops = DEFITER);
		void CBlake2BPLoop(size_t SampleSize, size_t Loops = DEFITER);
		void CppBlake2BLoop(size_t SampleSize, size_t Loops = DEFITER, bool Parallel = false);
		void CppBlake2BEx(size_t SampleSize, uint8_t Degree, size_t Loops = DEFITER);
		uint64_t GetBytesPerSecond(uint64_t DurationTicks, uint64_t DataSize);
		void OnProgress(char* Data);

		template<typename T>
		static inline std::string IntToString(const T& Value)
		{
			std::ostringstream oss;
			oss << Value;
			return oss.str();
		}
	};
}

#endif