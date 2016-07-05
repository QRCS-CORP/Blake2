#ifndef _BLAKE2TEST_BLAKETEST_H
#define _BLAKE2TEST_BLAKETEST_H

#include "ITest.h"

namespace BlakeTest
{
	/// <summary>
	/// Tests the Blake2 digest implementation using vector comparisons.
	/// <para>Tests all vectors from the official Blake2 submission:
	/// <see href="https://github.com/BLAKE2/BLAKE2/tree/master/testvectors"/></para>
	/// </summary>
	class Blake2Test : public ITest
	{
	private:
		const std::string DESCRIPTION = "Blake Vector KATs; tests Blake 256/512 digests.";
		const std::string FAILURE = "FAILURE! ";
		const std::string SUCCESS = "SUCCESS! All Blake tests have executed succesfully.";
		const std::string DMK_INP = "in:	";
		const std::string DMK_KEY = "key:	";
		const std::string DMK_HSH = "hash:	";

		std::vector<std::vector<uint8_t>> m_expected;
		std::vector<std::vector<uint8_t>> m_message;
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
		/// Blake2 Vector KATs from the official submission package
		/// </summary>
		Blake2Test()
		{
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~Blake2Test() 
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run();

	private:

		/*void BufferTest()
		{
			// debug test for update mechanism using odd sizes
			std::vector<uint8_t> input(2048);
			std::vector<uint8_t> key(64);
			std::vector<uint8_t> hash(64);
			Blake2::MacParams mkey(key);
			Blake2::Blake2Bp512 blake2b(true);

			for (size_t i = 0; i < 256; ++i)
			{
				input[i] = i;
				input[i + 256] = i;
				input[i + 512] = i;
				input[i + 768] = i;
				input[i + 1024] = i;
				input[i + 1280] = i;
				input[i + 1536] = i;
				input[i + 1792] = i;
			}

			blake2b.LoadMacKey(mkey);

			for (size_t i = 0; i < input.size(); ++i)
				blake2b.BlockUpdate(input, 0, i);

			blake2b.BlockUpdate(input, 0, 1024);
			blake2b.BlockUpdate(input, 0, 193);
			blake2b.BlockUpdate(input, 0, 512);
			blake2b.BlockUpdate(input, 0, 2048);
			blake2b.BlockUpdate(input, 0, 513);
			blake2b.BlockUpdate(input, 0, 256);
			blake2b.BlockUpdate(input, 0, 128);
			blake2b.BlockUpdate(input, 0, 65);
			blake2b.BlockUpdate(input, 0, 33);
			blake2b.BlockUpdate(input, 0, 3);
			blake2b.BlockUpdate(input, 0, 512);
			blake2b.BlockUpdate(input, 0, 7);

			blake2b.DoFinal(hash, 0);
		}

		void GenerateTest()
		{
			// debug test for drbg
			{
				std::vector<uint8_t> key(32);
				for (size_t i = 0; i < key.size(); ++i)
					key[i] = i;
				Blake2::MacParams mkey(key);
				Blake2::Blake2Sp256 blake2S(false);
				std::vector<uint8_t> rand(193);
				blake2S.Generate(mkey, rand);
			}

			{
				std::vector<uint8_t> key(64);
				for (size_t i = 0; i < key.size(); ++i)
					key[i] = i;
				Blake2::MacParams mkey(key);
				Blake2::Blake2Bp512 blake2B(false);
				std::vector<uint8_t> rand2(187);
				blake2B.Generate(mkey, rand2);
			}
		}*/

		void Blake2BTest();
		void Blake2BPTest();
		void Blake2STest();
		void Blake2SPTest();
		void Blake2BRandomSampleTest();
		void Blake2BPRandomSampleTest();
		void Blake2SRandomSampleTest();
		void Blake2SPRandomSampleTest();
		void MacParamsTest();
		void TreeParamsTest();
		void OnProgress(char* Data);
	};
}
#endif
