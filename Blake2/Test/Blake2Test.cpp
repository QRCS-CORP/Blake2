#include "Blake2Test.h"
#include <fstream>
#include <string.h>
#include "HexConverter.h"
#include "CSPRsg.h"

#include "../Blake2/Blake256.h"
#include "../Blake2/Blake512.h"
#include "../BlakeC/blake2.h"
#include "../Blake2/HMAC.h"
#include "../Blake2/SymmetricKey.h"

namespace TestBlake2
{
	using namespace CEX::Digest;
	using CEX::Key::Symmetric::SymmetricKey;

	std::string Blake2Test::Run()
	{
		try
		{
			TreeParamsTest();
			OnProgress("Passed BlakeParams parameter serialization test..");
			Blake2STest();
			OnProgress("Passed Blake2-S 256 vector tests..");
			Blake2SPTest();
			OnProgress("Passed Blake2-SP 256 vector tests..");
			Blake2BTest();
			OnProgress("Passed Blake2-B 512 vector tests..");
			Blake2BPTest();
			OnProgress("Passed Blake2-BP 512 vector tests..");    
			OnProgress("");/**/

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
			OnProgress("Passed 1000 rounds of Blake2SP random sample comparisons..");/**/

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

					Blake512 blake2b(false);
					blake2b.Initialize(CEX::Key::Symmetric::SymmetricKey(key));
					blake2b.Compute(input, hash);

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

					// Note: the official default is 4 threads, my default on all digests is 8 threads
					BlakeParams params(64, 2, 4, 0, 64);
					Blake512 blake2bp(params);
					blake2bp.Initialize(CEX::Key::Symmetric::SymmetricKey(key));
					blake2bp.Compute(input, hash);

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

			Blake512 blake2b(false);
			blake2b.Compute(input, hash1);

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
			uint16_t blkSize = 0;
			// get a random block size
			memcpy(&blkSize, &rnd.GetBytes(2)[0], 2);
			if (blkSize == 0)
				++blkSize;
			// get p-rand
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			blake2bp_init(S, 64);
			blake2bp_update(S, input.data(), input.size());
			blake2bp_final(S, hash2.data(), hash2.size());

			BlakeParams params(64, 2, 4, 0, 64);
			Blake512 blake2b(params);
			blake2b.Compute(input, hash1);

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

					Blake256 blake2s(false);
					blake2s.Initialize(CEX::Key::Symmetric::SymmetricKey(key));
					blake2s.Compute(input, hash);

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

					Blake256 blake2sp(true);
					blake2sp.Initialize(CEX::Key::Symmetric::SymmetricKey(key));
					blake2sp.Compute(input, hash);

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

			Blake256 blake2s(false);
			blake2s.Compute(input, hash1);

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
			uint16_t blkSize = 0;
			// get a random block size
			memcpy(&blkSize, &rnd.GetBytes(2)[0], 2);
			if (blkSize == 0)
				++blkSize;
			// get p-rand
			std::vector<uint8_t> input = rnd.GetBytes(blkSize);

			Blake256 blake2s(true);
			blake2s.Compute(input, hash1);

			blake2sp_init(S, 32);
			blake2sp_update(S, input.data(), input.size());
			blake2sp_final(S, hash2.data(), hash2.size());

			if (hash1 != hash2)
				throw std::string("Blake2SPTest: Random sample test has failed!");
		}
	}

	void Blake2Test::TreeParamsTest()
	{
		std::vector<byte> code1(40, 7);

		BlakeParams tree1(64, 64, 2, 1, 64000, 64, 1, 32, code1);
		std::vector<uint8_t> tres = tree1.ToBytes();
		BlakeParams tree2(tres);

		if (!tree1.Equals(tree2))
			throw std::string("Blake2STest: Tree parameters test failed!");

		std::vector<byte> code2(12, 3);
		BlakeParams tree3(32, 32, 2, 1, 32000, 32, 1, 32, code1);
		tres = tree3.ToBytes();
		BlakeParams tree4(tres);

		if (!tree3.Equals(tree4))
			throw std::string("Blake2STest: Tree parameters test failed!");
	}

	void Blake2Test::OnProgress(char* Data)
	{
		m_progressEvent(Data);
	}
}