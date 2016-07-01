#include "Blake2Test.h"
#include <fstream>
#include <string.h>

#include "HexConverter.h"
#include "CSPRsg.h"

#include "../Blake2/BlakeS256.h"
#include "../Blake2/BlakeB512.h"
#include "../Blake2/MacParams.h"
#include "../BlakeC/blake2.h"

#if defined(_MSC_VER) 
// Microsoft C/C++-compatible compiler
#	include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__)) 
// GCC-compatible compiler, targeting x86/x86-64
#	include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__) 
// GCC-compatible compiler, targeting ARM with NEON
#	include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__) 
// GCC-compatible compiler, targeting ARM with WMMX
#	include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__)) 
// XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX
#	include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__) 
// GCC-compatible compiler, targeting PowerPC with SPE
#	include <spe.h>
#endif

using namespace Blake2;

namespace BlakeTest
{

	std::string Blake2Test::Run()
	{
		try
		{
			TreeParamsTest();
			OnProgress("Passed Blake2Tree parameter serialization test..");
			MacParamsTest();
			OnProgress("Passed MacParams cloning test..");
			Blake2STest();
			OnProgress("Passed Blake2-S 256 vector tests..");
			Blake2SPTest();
			OnProgress("Passed Blake2-SP 256 vector tests..");
			Blake2BTest();
			OnProgress("Passed Blake2-B 512 vector tests..");
			Blake2BPTest();
			OnProgress("Passed Blake2-BP 512 vector tests..");    
			OnProgress("");

			OnProgress("Each algorithm is tested 1000 times with pseudo random message arrays");
			OnProgress("These tests compare output between the official C versions and the CEX C++ versions");
			OnProgress("");
			Blake2BRandomSampleTest();
			OnProgress("Passed 1000 rounds of Blake2B random sample comparisons..");
			Blake2BPRandomSampleTest();
			OnProgress("Passed 1000 rounds of Blake2BP random sample comparisons..");
			Blake2SRandomSampleTest();
			OnProgress("Passed 1000 rounds of Blake2S random sample comparisons..");
			Blake2SPRandomSampleTest();
			OnProgress("Passed 1000 rounds of Blake2SP random sample comparisons..");

			return SUCCESS;
		}
		catch (std::string const& ex)
		{
			throw TestException(std::string(FAILURE + " : " + ex));
		}
		catch (...)
		{
			throw TestException(std::string(FAILURE + " : Internal Error"));
		}
	}

	void Blake2Test::Blake2BTest()
	{
		std::ifstream stream("Vectors/blake2b-kat.txt");
		if (!stream)
			std::cerr << "Could not open file" << std::endl;

		std::string line;

		while (std::getline(stream, line))
		{
			if (line.size() != 0)
			{
				if (line.find(DMK_INP) != std::string::npos)
				{
					std::vector<uint8_t> input(0);
					std::vector<uint8_t> expect(64);
					std::vector<uint8_t> key;
					std::vector<uint8_t> hash(64);

					size_t sze = DMK_INP.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), input);

					std::getline(stream, line);
					sze = DMK_KEY.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), key);

					std::getline(stream, line);
					sze = DMK_HSH.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), expect);

					Blake2::MacParams mkey(key);
					Blake2::BlakeB512 blake2b(false);
					blake2b.LoadMacKey(mkey);
					blake2b.ComputeHash(input, hash);

					if (hash != expect)
						throw std::string("Blake2BTest: KAT test has failed!");
				}
			}
		}
		stream.close();
	}

	void Blake2Test::Blake2BPTest()
	{
		std::ifstream stream("Vectors/blake2bp-kat.txt");
		if (!stream)
			std::cerr << "Could not open file" << std::endl;

		std::string line;

		while (std::getline(stream, line))
		{
			if (line.size() != 0)
			{
				if (line.find(DMK_INP) != std::string::npos)
				{
					std::vector<uint8_t> input(0);
					std::vector<uint8_t> expect(64);
					std::vector<uint8_t> key;
					std::vector<uint8_t> hash(64);
					std::vector<uint8_t> hash2(64);

					size_t sze = DMK_INP.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), input);

					std::getline(stream, line);
					sze = DMK_KEY.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), key);

					std::getline(stream, line);
					sze = DMK_HSH.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), expect);

					Blake2::BlakeB512 blake2(true);
					Blake2::MacParams mkey(key);
					blake2.LoadMacKey(mkey);
					blake2.ComputeHash(input, hash);

					if (hash != expect)
						throw std::string("Blake2BPTest: KAT test has failed!");
				}
			}
		}
		stream.close();
	}

	void Blake2Test::Blake2BRandomSampleTest()
	{
		CSPRsg rnd;
		std::vector<uint8_t> hash1(64);
		std::vector<uint8_t> hash2(64);
		blake2b_state S[1];

		for (size_t i = 0; i < 1000; ++i)
		{
			uint16_t blkSize = 0;
			// get a random block size
			memcpy(&blkSize, &rnd.GetBytes(2)[0], 2);
			if (blkSize == 0)
				++blkSize;
			// get p-rand
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			Blake2::BlakeB512 blake2b(false);
			blake2b.ComputeHash(input, hash1);

			blake2b_init(S, 64);
			blake2b_update(S, input.data(), input.size());
			blake2b_final(S, hash2.data(), hash2.size());

			if (hash1 != hash2)
				throw std::string("Blake2BTest: Random sample test has failed!");
		}
	}

	void Blake2Test::Blake2BPRandomSampleTest()
	{
		CSPRsg rnd;
		std::vector<uint8_t> hash1(64);
		std::vector<uint8_t> hash2(64);
		blake2bp_state S[1];
		std::vector<uint8_t> key(64);

		for (size_t i = 0; i < 1000; ++i)
		{
			uint16_t blkSize = 512;
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			Blake2::BlakeB512 blake2b(true);
			blake2b.ComputeHash(input, hash1);

			blake2bp_init(S, 64);
			blake2bp_update(S, input.data(), input.size());
			blake2bp_final(S, hash2.data(), hash2.size());

			if (hash1 != hash2)
				throw std::string("Blake2BPTest: Random sample test has failed!");
		}
	}

	void Blake2Test::Blake2STest()
	{
		std::ifstream stream("Vectors/blake2s-kat.txt");
		if (!stream)
			std::cerr << "Could not open file" << std::endl;

		std::string line;

		while (std::getline(stream, line))
		{
			if (line.size() != 0)
			{
				if (line.find(DMK_INP) != std::string::npos)
				{
					std::vector<uint8_t> input(0);
					std::vector<uint8_t> expect(32);
					std::vector<uint8_t> key;
					std::vector<uint8_t> hash(32);

					size_t sze = DMK_INP.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), input);

					std::getline(stream, line);
					sze = DMK_KEY.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), key);

					std::getline(stream, line);
					sze = DMK_HSH.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), expect);

					Blake2::MacParams mkey(key);
					Blake2::BlakeS256 blake2s(false);
					blake2s.LoadMacKey(mkey);
					blake2s.ComputeHash(input, hash);

					if (hash != expect)
						throw std::string("Blake2STest: KAT test has failed!");
				}
			}
		}
		stream.close();
	}

	void Blake2Test::Blake2SPTest()
	{
		std::ifstream stream("Vectors/blake2sp-kat.txt");
		if (!stream)
			std::cerr << "Could not open file" << std::endl;

		std::string line;

		while (std::getline(stream, line))
		{
			if (line.size() != 0)
			{
				if (line.find(DMK_INP) != std::string::npos)
				{
					std::vector<uint8_t> input(0);
					std::vector<uint8_t> expect(32);
					std::vector<uint8_t> key;
					std::vector<uint8_t> hash(32);

					size_t sze = DMK_INP.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), input);

					std::getline(stream, line);
					sze = DMK_KEY.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), key);

					std::getline(stream, line);
					sze = DMK_HSH.length();
					if (line.length() - sze > 0)
						HexConverter::Decode(line.substr(sze, line.length() - sze), expect);

					Blake2::MacParams mkey(key);
					Blake2::BlakeS256 blake2sp(true);
					blake2sp.LoadMacKey(mkey);
					blake2sp.ComputeHash(input, hash);

					if (hash != expect)
						throw std::string("Blake2SPTest: KAT test has failed!");
				}
			}
		}
		stream.close();
	}

	void Blake2Test::Blake2SRandomSampleTest()
	{
		CSPRsg rnd;
		std::vector<uint8_t> hash1(32);
		std::vector<uint8_t> hash2(32);
		blake2s_state S[1];

		for (size_t i = 0; i < 1000; ++i)
		{
			uint16_t blkSize = 0;
			// get a random block size
			memcpy(&blkSize, &rnd.GetBytes(2)[0], 2);
			if (blkSize == 0)
				++blkSize;
			// get p-rand
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			Blake2::BlakeS256 blake2s(false);
			blake2s.ComputeHash(input, hash1);

			blake2s_init(S, 32);
			blake2s_update(S, input.data(), input.size());
			blake2s_final(S, hash2.data(), hash2.size());

			if (hash1 != hash2)
				throw std::string("Blake2STest: Random sample test has failed!");
		}
	}

	void Blake2Test::Blake2SPRandomSampleTest()
	{
		CSPRsg rnd;
		std::vector<uint8_t> hash1(32);
		std::vector<uint8_t> hash2(32);
		blake2sp_state S[1];
		std::vector<uint8_t> key(32);

		for (size_t i = 0; i < 1000; ++i)
		{
			uint16_t blkSize = 512;
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			Blake2::BlakeS256 blake2s(true);
			blake2s.ParallelBlockSize() = blkSize;
			blake2s.ComputeHash(input, hash1);

			blake2sp_init(S, 32);
			blake2sp_update(S, input.data(), input.size());
			blake2sp_final(S, hash2.data(), hash2.size());

			if (hash1 != hash2)
				throw std::string("Blake2SPTest: Random sample test has failed!");
		}
	}

	void Blake2Test::MacParamsTest()
	{
		std::vector<uint8_t> key(64);
		for (size_t i = 0; i < key.size(); ++i)
			key[i] = i;

		Blake2::MacParams mkey(key, key, key);
		Blake2::MacParams* mkey2 = mkey.DeepCopy();
		Blake2::MacParams mkey3 = mkey.Clone();

		if (!mkey.Equals(*mkey2) || !mkey.Equals(mkey3))
			throw std::string("Blake2STest: Mac parameters test failed!");
	}

	void Blake2Test::TreeParamsTest()
	{
		Blake2::Blake2Tree tree1(64, 64, 2, 1, 64000, 64, 1, 32, 0);
		std::vector<uint8_t> tres = tree1.ToBytes();
		Blake2::Blake2Tree tree2(tres);

		if (!tree1.Equals(tree2))
			throw std::string("Blake2STest: Tree parameters test failed!");
	}

	void Blake2Test::OnProgress(char* Data)
	{
		m_progressEvent(Data);
	}
}